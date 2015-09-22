#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

void
base64_encode(const unsigned char* s, size_t length, char** out)
{
	BIO *bio, *b64;
	BUF_MEM *buffer;

	b64 = BIO_new(BIO_f_base64());
	bio = BIO_new(BIO_s_mem());
	bio = BIO_push(b64, bio);

	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
	BIO_write(bio, s, length);
	BIO_flush(bio);
	BIO_get_mem_ptr(bio, &buffer);
	BIO_set_close(bio, BIO_NOCLOSE);
	BIO_free_all(bio);

	*out = buffer->data;
}
