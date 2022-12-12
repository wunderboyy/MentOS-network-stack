

#define LITTLETOBIG16(x) ((((x)&0xff00) >> 8) | (((x)&0x00ff) << 8))

#define BIGTOLITTLE16(x) ((((x)&0xff) << 8) | (((x)&0xff00) >> 8))
