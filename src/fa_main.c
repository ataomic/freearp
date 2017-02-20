#include <stdio.h>
#include <fa_net.h>

int main(int argc, char** argv)
{
	FILE* fp;
	char buff[256], *s, *end;
	struct fa_ctx ctx;
	
	if(argc < 1)
		return -1;
	
	for(;;) {
		fp = fopen(argv[1], "rt");
		if(!fp)
			break;
		while(fgets(buff, sizeof(buff), fp) != NULL) {
			if(fa_ctx_read(&ctx, buff) != 0)
				continue;
			fa_send(&ctx);
			if(ctx.interval)
				sleep(ctx.interval);
		}
		fclose(fp);
		sleep(1);
	}

	return 0;
}
