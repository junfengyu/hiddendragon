#include <sys/types.h>
#include <sys/stat.h>

//#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "mman.h"
#include "config.h"

#include "arc.h"
#include "outguess.h"
#include "golay.h"
#include "pnm.h"
#include "jpg.h"
#include "iterator.h"

#ifndef MAP_FAILED
/* Some Linux systems are missing this */
#define MAP_FAILED	(void *)-1
#endif /* MAP_FAILED */


uint16_t htons (uint16_t x)
{
   #if BYTE_ORDER == BIG_ENDIAN
    return x;
   #elif BYTE_ORDER == LITTLE_ENDIAN
    return __bswap_16 (x);
    #else
    # error "What kind of system is this?"
    #endif
}


uint32_t htonl (uint32_t x)

{
    #if BYTE_ORDER == BIG_ENDIAN
      return x;
    #elif BYTE_ORDER == LITTLE_ENDIAN
     return __bswap_32 (x);
    #else
    # error "What kind of system is this?"
    #endif
}



static int steg_err_buf[CODEBITS];
static int steg_err_cnt;
static int steg_errors;
static int steg_encoded;

int steg_offset[MAX_SEEK];
int steg_foil;
int steg_foilfail;

static int steg_count;
static int steg_mis;
static int steg_mod;
static int steg_data;

/* Exported variables */

int steg_stat;

/* format handlers */


void *
checkedmalloc(size_t n)
{
    void *p;

    if (!(p = malloc(n))) {
        fprintf(stderr, "checkedmalloc: not enough memory\n");
        exit(1);
    }

    return p;
}

void
mmap_file(char *name, u_char **data, int *size)
{
    int fd;
    struct stat fs;
    char *p;

    if ((fd = open(name, O_RDONLY, 0)) == -1) {
        fprintf(stderr, "Can not open %s\n", name);
        exit(1);
    }

    if (fstat(fd, &fs) == -1) {
        perror("fstat");
        exit(1);
    }

#ifdef HAVE_MMAP
    if ((p = mmap(NULL, fs.st_size, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
#else
    p = checkedmalloc(fs.st_size);
    if (read(fd, p, fs.st_size) != fs.st_size) {
        perror("read");
        exit(1);
    }
#endif /* HAVE_MMAP */
    close(fd);

    *data = p;
    *size = fs.st_size;
}

void
munmap_file(u_char *data, int len)
{
#ifdef HAVE_MMAP
    if (munmap(data, len) == -1) {
        perror("munmap");
        exit(1);
    }
#else
    free (data);
#endif /* HAVE_MMAP */
}


/*
 * The error correction might allow us to introduce extra errors to
 * avoid modifying data.  Choose to leave bits with high detectability
 * untouched.
 */

void
steg_adjust_errors(bitmap *bitmap, int flags)
{
    int i, j, n, many, flag;
    int priority[ERRORBITS], detect[ERRORBITS];

    many = ERRORBITS - steg_errors;
    for (j = 0; j < many && j < steg_err_cnt; j++) {
        priority[j] = steg_err_buf[j];
        detect[j] = bitmap->detect[priority[j]];
    }

    /* Very simple sort */
    do {
        for (flag = 0, i = 0; i < j - 1; i++)
            if (detect[i] < detect[i + 1]) {
                SWAP(detect[i], detect[i+1]);
                SWAP(priority[i], priority[i+1]);
                flag = 1;
            }
    } while (flag);

    for (i = j; i < steg_err_cnt; i++) {
        for (n = 0; n < j; n++)
            if (detect[n] < bitmap->detect[steg_err_buf[i]])
                break;
        if (n < j - 1) {
            memmove(detect + n + 1, detect + n,
                (j - n) * sizeof(int));
            memmove(priority + n + 1, priority + n,
                (j - n) * sizeof(int));
        }
        if (n < j) {
            priority[n] = steg_err_buf[i];
            detect[n] = bitmap->detect[steg_err_buf[i]];
        }
    }

    for (i = 0; i < j; i++) {
        if (flags & STEG_EMBED) {
            WRITE_BIT(bitmap->locked, i, 0);
            if (TEST_BIT(bitmap->bitmap, priority[i]))
                WRITE_BIT(bitmap->bitmap, i, 0);
            else
                WRITE_BIT(bitmap->bitmap, i, 1);
        }
        steg_mis--;
        steg_mod -= detect[i];
    }
}

int
steg_embedchunk(bitmap *bitmap, iterator *iter,
        u_int32_t data, int bits, int embed)
{
    int i = ITERATOR_CURRENT(iter);
    u_int8_t bit;
    u_int32_t val;
    u_char *pbits, *plocked;
    int nbits;

    pbits = bitmap->bitmap;
    plocked = bitmap->locked;
    nbits = bitmap->bits;

    while (i < nbits && bits) {
        if ((embed & STEG_ERROR) && !steg_encoded) {
            if (steg_err_cnt > 0)
                steg_adjust_errors(bitmap, embed);
            steg_encoded = CODEBITS;
            steg_errors = 0;
            steg_err_cnt = 0;
            memset(steg_err_buf, 0, sizeof(steg_err_buf));
        }
        steg_encoded--;

        bit = TEST_BIT(pbits, i) ? 1 : 0;
        val = bit ^ (data & 1);
        steg_count++;
        if (val == 1) {
            steg_mod += bitmap->detect[i];
            steg_mis++;
        }

        /* Check if we are allowed to change a bit here */
        if ((val == 1) && TEST_BIT(plocked, i)) {
            if (!(embed & STEG_ERROR) || (++steg_errors > 3))
                return 0;
            val = 2;
        }

        /* Store the bits we changed in error encoding mode */
        if ((embed & STEG_ERROR) && val == 1)
            steg_err_buf[steg_err_cnt++] = i;

        if (val != 2 && (embed & STEG_EMBED)) {
            WRITE_BIT(plocked, i, 1);
                WRITE_BIT(pbits, i, data & 1);
        }

        data >>= 1;
        bits--;

        i = iterator_next(iter, bitmap);
    }

    return 1;
}

stegres
steg_embed(bitmap *bitmap, iterator *iter, struct arc4_stream *as,
       u_char *data, u_int datalen, u_int16_t seed, int embed)
{
    int i, len;
    u_int32_t tmp = 0;
    u_char tmpbuf[4], *encbuf;
    stegres result;

    steg_count = steg_mis = steg_mod = 0;

    memset(&result, 0, sizeof(result));

    if (bitmap->bits / (datalen * 8) < 2) {
        fprintf(stderr, "steg_embed: not enough bits in bitmap "
            "for embedding: %d > %d/2\n",
            datalen * 8, bitmap->bits);
		result.error="no enough bits in bitmap";
        return result;
    }

    if (embed & STEG_EMBED)
        fprintf(stderr, "Embedding data: %d in %d\n",
            datalen * 8, bitmap->bits);

    /* Clear error counter */
    steg_encoded = 0;
    steg_err_cnt = 0;

    /* Encode the seed and datalen */
    tmpbuf[0] = seed & 0xff;
    tmpbuf[1] = seed >> 8;
    tmpbuf[2] = datalen & 0xff;
    tmpbuf[3] = datalen >> 8;

    /* Encode the admin data XXX maybe derive another stream */
    len = 4;
    encbuf = encode_data (tmpbuf, &len, as, embed);

    for (i = 0; i < len; i++)
        if (!steg_embedchunk(bitmap, iter, encbuf[i], 8, embed)) {
            free (encbuf);

            /* If we use error correction or a bit in the seed
             * was locked, we can go on, otherwise we have to fail.
             */
            if ((embed & STEG_ERROR) ||
                steg_count < 16 /* XXX */)
                result.error = STEG_ERR_HEADER;
            else
                result.error = STEG_ERR_PERM;
            return result;
        }
    free (encbuf);

    /* Clear error counter again, a new ECC block starts */
    steg_encoded = 0;

    iterator_seed(iter, bitmap, seed);

    while (ITERATOR_CURRENT(iter) < bitmap->bits && datalen > 0) {
        iterator_adapt(iter, bitmap, datalen);

        tmp = *data++;
        datalen--;

        if (!steg_embedchunk(bitmap, iter, tmp, 8, embed)) {
            result.error = STEG_ERR_BODY;
            return result;
        }
    }

    /* Final error adjustion after end */
    if ((embed & STEG_ERROR) && steg_err_cnt > 0)
      steg_adjust_errors(bitmap, embed);

    if (embed & STEG_EMBED) {
        fprintf(stderr, "Bits embedded: %d, "
            "changed: %d(%2.1f%%)[%2.1f%%], "
            "bias: %d, tot: %d, skip: %d\n",
            steg_count, steg_mis,
            (float) 100 * steg_mis/steg_count,
            (float) 100 * steg_mis/steg_data, /* normalized */
            steg_mod,
            ITERATOR_CURRENT(iter),
            ITERATOR_CURRENT(iter) - steg_count);
    }

    result.changed = steg_mis;
    result.bias = steg_mod;

    return result;
}

u_int32_t
steg_retrbyte(bitmap *bitmap, int bits, iterator *iter)
{
    u_int32_t i = ITERATOR_CURRENT(iter);
    int where;
    u_int32_t tmp = 0;

    for (where = 0; where < bits; where++) {
        tmp |= (TEST_BIT(bitmap->bitmap, i) ? 1 : 0) << where;

        i = iterator_next(iter, bitmap);
    }

    return tmp;
}

char *
steg_retrieve(int *len, bitmap *bitmap, iterator *iter, struct arc4_stream *as,
          int flags)
{
    u_int32_t n;
    int i;
    u_int32_t origlen;
    u_int16_t seed;
    u_int datalen;
    u_char *buf;
    u_int8_t *tmpbuf;


    datalen = 4;
    encode_data(NULL, &datalen, NULL, flags);
    tmpbuf = checkedmalloc(datalen);

    for (i = 0; i < datalen; i++)
        tmpbuf[i] = steg_retrbyte(bitmap, 8, iter);

    buf = decode_data (tmpbuf, &datalen, as, flags);

    if (datalen != 4) {
        fprintf (stderr, "Steg retrieve: wrong data len: %d\n",
             datalen);
        exit (1);
    }

    free (tmpbuf);

    seed = buf[0] | (buf[1] << 8);
    origlen = datalen = buf[2] | (buf[3] << 8);

    free (buf);

    fprintf(stderr, "Steg retrieve: seed: %d, len: %d\n", seed, datalen);

    if (datalen > bitmap->bytes) {
        fprintf(stderr, "Extracted datalen is too long: %d > %d\n",
            datalen, bitmap->bytes);
        exit(1);
    }

    buf = checkedmalloc(datalen);

    iterator_seed(iter, bitmap, seed);

    n = 0;
    while (datalen > 0) {
        iterator_adapt(iter, bitmap, datalen);
        buf[n++] = steg_retrbyte(bitmap, 8, iter);
        datalen --;
    }

    *len = origlen;
    return buf;
}

int
steg_find(bitmap *bitmap, iterator *iter, struct arc4_stream *as,
      int siter, int siterstart,
      u_char *data, int datalen, int flags)
{
    int changed, tch, half, chmax, chmin;
    int j, i, size = 0;
    struct arc4_stream tas;
    iterator titer;
    u_int16_t *chstats = NULL;
    stegres result;

    half = datalen * 8 / 2;

    if (!siter && !siterstart)
        siter = DEFAULT_ITER;

    if (siter && siterstart < siter) {
        if (steg_stat) {
            /* Collect stats about changed bit */
            size = siter - siterstart;
            chstats = checkedmalloc(size * sizeof(u_int16_t));
            memset(chstats, 0, size * sizeof(u_int16_t));
        }

        fprintf(stderr, "Finding best embedding...\n");
        changed = chmin = chmax = -1; j = -STEG_ERR_HEADER;

        for (i = siterstart; i < siter; i++) {
            titer = *iter;
            tas = *as;
            result = steg_embed(bitmap, &titer, &tas,
                     data, datalen, i, flags);
            /* Seed does not effect any more */
            if (result.error == STEG_ERR_PERM)
                return -result.error;
            else if (result.error)
                continue;

            /*
             * Only count bias, if we do not modifiy many
             * extra bits for statistical foiling.
             */
            tch = result.changed + result.bias;

            if (steg_stat)
                chstats[i - siterstart] = result.changed;

            if (chmax == -1 || result.changed > chmax)
                chmax = result.changed;
            if (chmin == -1 || result.changed < chmin)
                chmin = result.changed;

            if (changed == -1 || tch < changed) {
                changed = tch;
                j = i;
                fprintf(stderr, "%5u: %5u(%3.1f%%)[%3.1f%%], bias %5d(%1.2f), saved: % 5d, total: %5.2f%%\n",
                    j, result.changed,
                    (float) 100 * steg_mis / steg_count,
                    (float) 100 * steg_mis / steg_data,
                    result.bias,
                    (float)result.bias / steg_mis,
                    (half - result.changed) / 8,
                    (float) 100 * steg_mis / bitmap->bits);
            }
        }

        if (steg_stat && (chmax - chmin > 1)) {
            double mean = 0, dev, sq;
            int cnt = 0, count = chmax - chmin + 1;
            u_int16_t *chtab;
            int chtabcnt = 0;

            chtab = checkedmalloc(count * sizeof(u_int16_t));
            memset(chtab, 0, count * sizeof(u_int16_t));

            for (i = 0; i < size; i++)
                if (chstats[i] > 0) {
                    mean += chstats[i];
                    cnt++;
                    chtab[chstats[i] - chmin]++;
                    chtabcnt++;
                }

            mean = mean / cnt;
            dev = 0;
            for (i = 0; i < size; i++)
                if (chstats[i] > 0) {
                    sq = chstats[i] - mean;
                    dev += sq * sq;
                }

            fprintf(stderr, "Changed bits. Min: %d, Mean: %f, +- %f, Max: %d\n",
                chmin,
                mean, sqrt(dev / (cnt - 1)),
                chmax);

            if (steg_stat > 1)
                for (i = 0; i < count; i++) {
                    if (!chtab[i])
                        continue;
                    fprintf(stderr, "%d: %.9f\n",
                        chmin + i,
                        (double)chtab[i]/chtabcnt);
                }

            free (chtab);
            free (chstats);
        }

        fprintf(stderr, "%d, %d: ", j, changed);
    } else
        j = siterstart;

    return j;
}

/* graphic file handling routines */

u_char *
encode_data(u_char *data, int *len, struct arc4_stream *as, int flags)
{
    int j, datalen = *len;
    u_char *encdata;

    if (flags & STEG_ERROR) {
        int eclen, i = 0, length = 0;
        u_int32_t tmp;
        u_int64_t code = 0;
        u_char edata[3];

        datalen = datalen + (3 - (datalen % 3));
        eclen = (datalen * 8 / DATABITS * CODEBITS + 7)/ 8;

        if (data == NULL) {
            *len = eclen;
            return NULL;
        }

        encdata = checkedmalloc(3 * eclen * sizeof(u_char));
        while (datalen > 0) {
            if (datalen > 3)
                memcpy(edata, data, 3);
            else {
                int adj = *len % 3;
                memcpy (edata, data, adj);

                /* Self describing padding */
                for (j = 2; j >= adj; j--)
                    edata[j] = j - adj;
            }
            tmp = edata[0];
            tmp |= edata[1] << 8;
            tmp |= edata[2] << 16;

            data += 3;
            datalen -= 3;

            for (j = 0; j < 2; j++) {
                code |= ENCODE(tmp & DATAMASK) << length;
                length += CODEBITS;
                while (length >= 8) {
                    encdata[i++] = code & 0xff;
                    code >>= 8;
                    length -= 8;
                }
                tmp >>= DATABITS;
            }
        }

        /* Encode the rest */
        if (length > 0)
            encdata[i++] = code & 0xff;

        datalen = eclen;
        data = encdata;
    } else {
        if (data == NULL) {
            *len = datalen;
            return NULL;
        }
        encdata = checkedmalloc(datalen * sizeof(u_char));
    }

    /* Encryption */
    for (j = 0; j < datalen; j++)
        encdata[j] = data[j] ^ arc4_getbyte(as);

    *len = datalen;

    return encdata;
}

u_char * decode_data(u_char *encdata, int *len, struct arc4_stream *as, int flags)
{
    int i, j, enclen = *len, declen;
    u_char *data;

    for (j = 0; j < enclen; j++)
        encdata[j] = encdata[j] ^ arc4_getbyte(as);

    if (flags & STEG_ERROR) {
        u_int32_t inbits = 0, outbits = 0, etmp, dtmp;

        declen = enclen * DATABITS / CODEBITS;
        data = checkedmalloc(declen * sizeof(u_char));

        etmp = dtmp = 0;
        for (i = 0, j = 0; i < enclen && j < declen; ) {
            while (outbits < CODEBITS) {
                etmp |= TDECODE(encdata + i, enclen)<< outbits;
                i++;
                outbits += 8;
            }
            dtmp |= (DECODE(etmp & CODEMASK) >>
                 (CODEBITS - DATABITS)) << inbits;
            inbits += DATABITS;
            etmp >>= CODEBITS;
            outbits -= CODEBITS;
            while (inbits >= 8) {
                data[j++] = dtmp & 0xff;
                dtmp >>= 8;
                inbits -= 8;
            }
        }

        i = data[declen -1];
        if (i > 2) {
            fprintf (stderr, "decode_data: padding is incorrect: %d\n",
                 i);
            *len = 0;
            return data;
        }
        for (j = i; j >= 0; j--)
            if (data[declen - 1 - i + j] != j)
                break;
        if (j >= 0) {
            fprintf (stderr, "decode_data: padding is incorrect: %d\n",
                 i);
            *len = 0;
            return data;
        }

        declen -= i + 1;
        fprintf (stderr, "Decode: %d data after ECC: %d\n",
             *len, declen);

    } else {
        data = checkedmalloc(enclen * sizeof(u_char));
        declen = enclen;
        memcpy (data, encdata, declen);
    }

    *len = declen;
    return data;
}

int do_embed(bitmap *bitmap, u_char *filename, u_char *key, u_int klen,
     config *cfg, stegres *result)
{
    iterator iter;
    struct arc4_stream as, tas;
    u_char *encdata, *data;
    u_int datalen, enclen;
    size_t correctlen;
    int j;

    /* Initialize random data stream */
    arc4_initkey(&as,  "Encryption", key, klen);
    tas = as;

    iterator_init(&iter, bitmap, key, klen);

    /* Encode the data for us */
    mmap_file(filename, &data, &datalen);
    steg_data = datalen * 8;
    enclen = datalen;
    encdata = encode_data(data, &enclen, &tas, cfg->flags);
    if (cfg->flags & STEG_ERROR) {
        fprintf(stderr, "Encoded '%s' with ECC: %d bits, %d bytes\n",
            filename, enclen * 8, enclen);
        correctlen = enclen / 2 * 8;
    } else {
        fprintf(stderr, "Encoded '%s': %d bits, %d bytes\n",
            filename, enclen * 8, enclen);
        correctlen = enclen * 8;
    }
    if (bitmap->maxcorrect && correctlen > bitmap->maxcorrect) {
        fprintf(stderr, "steg_embed: "
            "message larger than correctable size %d > %d\n",
            correctlen, bitmap->maxcorrect);
        return -1;
    }

    munmap_file(data, datalen);

    j = steg_find(bitmap, &iter, &as, cfg->siter, cfg->siterstart,
              encdata, enclen, cfg->flags);
    if (j < 0) {
        fprintf(stderr, "Failed to find embedding.\n");
        goto out;
    }

    *result = steg_embed(bitmap, &iter, &as, encdata, enclen, j,
                cfg->flags | STEG_EMBED);

 out:
    free(encdata);

    return (j);
}



