#include <stdio.h>
#include <stdlib.h>
#define DEFAULTDEV "/dev/sda"

void handleEFIEntry(int time){
	int suffix = 1024+(128*time);
	FILE *f = fopen(DEFAULTDEV,"rb");
	if(f){
		fseek(f,suffix,SEEK_SET);
		printf("EFI %ith entry:\n",time);
		printf("\tTYPE GUID\t=");
		for(int i = 0 ; i < 16 ; i++){
			unsigned char x = fgetc(f);
			if(x==0x00&&i==0){goto empty;}
			printf(" %x",x);
		}
		printf("\n");
		printf("\tUNIQUE GUID\t=");
		for(int i = 0 ; i < 16 ; i++){
			printf(" %x",fgetc(f));
		}
		printf("\n");
		printf("\tFIRST LBA\t=");
		for(int i = 0 ; i < 8 ; i++){
			printf(" %x",fgetc(f));
		}
		printf("\n");
		printf("\tLAST LBA\t=");
		for(int i = 0 ; i < 8 ; i++){
			printf(" %x",fgetc(f));
		}
		printf("\n");
		printf("\tATTRIBUTE\t=");
		for(int i = 0 ; i < 8 ; i++){
			printf(" %x",fgetc(f));
		}
		printf("\n");
		printf("\tTITLE\t\t= ");
		for(int i = 0 ; i < 72 ; i++){
			printf("%c",fgetc(f));
		}
		printf("\n");
		empty:
		printf("\n");
		fclose(f);
	}
}

void handleEFI(){
	handleEFIEntry(0);
	handleEFIEntry(1);
	handleEFIEntry(2);
	handleEFIEntry(3);
	handleEFIEntry(4);
	handleEFIEntry(5);
}


void discoverMBREntry(long int suffix){
	FILE *f = fopen(DEFAULTDEV,"rb");
	if(f){
		fseek(f,suffix,SEEK_SET);
		char opstartbaan = fgetc(f);
		fseek(f,suffix + 4,SEEK_SET);
		char sysid = fgetc(f);
		fseek(f,suffix + 8,SEEK_SET);
		char locA = fgetc(f);
		char locB = fgetc(f);
		char locC = fgetc(f);
		char locD = fgetc(f);
		printf("Offset: %x | Bootable: %s | systeemid: %s \t | lba: %x %x %x %x \n",suffix,opstartbaan==0x80?"yes":"no",(sysid & 0xff)==0xee?"UEFI Sector specificatie":"unknown",locA & 0xff , locB & 0xff , locC & 0xff, locD & 0xff);
		if((sysid & 0xff)==0xee){
			handleEFI();
		}else{
			printf("unknown filesystem");
		}
		printf("\n");
		fclose(f);
	}
}

void main(int a,char** b){
	printf("====================================\n");
	printf("  MASTERBOOTRECORD INFORMATIONTABLW\n");
	printf("------------------------------------\n\n");
	FILE *f = fopen(DEFAULTDEV,"rb");
	if(f){
		fseek(f,0x1fe,SEEK_SET);
		char signA = fgetc(f) & 0xff;
		char signB = fgetc(f) & 0xff;
		printf("The bootsignature is %x %x \n",signA,signB & 0xff);
		printf("The name is : ");
		fseek(f,0x1b4,SEEK_SET);
		for(int i = 0 ; i < 10 ; i++){
			printf("%c",fgetc(f) & 0xff);
		}
		printf("\n");
		printf("The following MBRentries are detected: \n");
		fclose(f);
		discoverMBREntry(0x01BE);
		discoverMBREntry(0x01CE);
		discoverMBREntry(0x01DE);
		discoverMBREntry(0x01EE);
		printf("OKAY!\n");
	}
	
}
