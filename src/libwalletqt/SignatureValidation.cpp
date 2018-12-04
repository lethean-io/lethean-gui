#include "SignatureValidation.h"

static void
edassert(int check, int round, const char *failreason) {
    if (check)
        return;
    printf("round %d, %s\n", round, failreason);
    exit(1);
}

static void
edassert_die(const unsigned char *a, const unsigned char *b, size_t len, int round, const char *failreason) {
    size_t i;
    if (round > 0)
        printf("round %d, %s\n", round, failreason);
    else
        printf("%s\n", failreason);
    printf("want: "); for (i = 0; i < len; i++) printf("%02x,", a[i]); printf("\n");
    printf("got : "); for (i = 0; i < len; i++) printf("%02x,", b[i]); printf("\n");
    printf("diff: "); for (i = 0; i < len; i++) if (a[i] ^ b[i]) printf("%02x,", a[i] ^ b[i]); else printf("  ,"); printf("\n\n");
    exit(1);
}


static void
edassert_equal_round(const unsigned char *a, const unsigned char *b, size_t len, int round, const char *failreason) {
    if (memcmp(a, b, len) == 0)
        return;
    edassert_die(a, b, len, round, failreason);
}


/* test data */
typedef struct test_data_t {
    unsigned char sk[32], pk[32], sig[64];
    const char *m;
} test_data;


test_data dataset[] = {
#include "Validation.h"
};

bool SignatureValidation::verify()
{

    ed25519_public_key pk;
    ed25519_signature sig;
    unsigned char forge[1024] = {'x'};

    int i;
    int stringLen = 29145;
    for (i = 0; i < 1; i++) {
        ed25519_publickey(dataset[i].sk, pk);
        edassert_equal_round(dataset[i].pk, pk, sizeof(pk), i, "public key didn't match");
        ed25519_sign((unsigned char *)dataset[i].m, stringLen, dataset[i].sk, pk, sig);
        edassert_equal_round(dataset[i].sig, sig, sizeof(sig), i, "signature didn't match");
        edassert(!ed25519_sign_open((unsigned char *)dataset[i].m, stringLen, pk, sig), i, "failed to open message");

        memcpy(forge, dataset[i].m, i);
        if (i)
            forge[i - 1] += 1;

        edassert(ed25519_sign_open(forge, (i) ? i : 1, pk, sig), i, "opened forged message");
    }

    qDebug() << "signature OK";

    return 0;
}
