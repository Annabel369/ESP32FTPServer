// Certificado ECC - Identidade Digital (Validade 10 anos)
// Use exatamente assim, com o conteúdo que você gerou:
const char server_crt[] = 
"-----BEGIN CERTIFICATE-----\n"
"MIIBgDCCASegAwIBAgIURUQJyfqssYPUqY2tlhZBeZorlf8wCgYIKoZIzj0EAwIw\n"
"FjEUMBIGA1UEAwwLZXNwMzIubG9jYWwwHhcNMjYwMTE3MDEyNzI3WhcNMzYwMTE1\n"
"MDEyNzI3WjAWMRQwEgYDVQQDDAtlc3AzMi5sb2NhbDBZMBMGByqGSM49AgEGCCqG\n"
"SM49AwEHA0IABJslyof84POqbw3PFNghYHE80BjVzkdE8DkkAS8gmvfmS6fVXhLH\n"
"W1UtvJc00Sg0tfniwxPpS66uGGc99MSjfDGjUzBRMB0GA1UdDgQWBBSIk88EzOwa\n"
"EpP32YLmi7OrbmUEcjAfBgNVHSMEGDAWgBSIk88EzOwaEpP32YLmi7OrbmUEcjAP\n"
"BgNVHRMBAf8EBTADAQH/MAoGCCqGSM49BAMCA0cAMEQCICHG7FsXhgm2VDTwXiWV\n"
"sTo3Mxb91fAA/Ct3lFywOCIkAiAwQQblHydfpabCNW5xXn4XH9vc/RrXmYJilPqW\n"
"3lxyZA==\n"
"-----END CERTIFICATE-----"; // Removido o \n final e a barra invertida

const char server_key[] = 
"-----BEGIN EC PRIVATE KEY-----\n"
"MHcCAQEEIP0mme/SRTp+/xCcqncNMsUYyegdL4lK1RU+9uZi8Xr8oAoGCCqGSM49\n"
"AwEHoUQDQgAEmyXKh/zg86pvDc8U2CFgcTzQGNXOR0TwOSQBLyCa9+ZLp9VeEsdb\n"
"VS28lzTRKDS1+eLDE+lLrq4YZz30xKN8MQ==\n"
"-----END EC PRIVATE KEY-----";
