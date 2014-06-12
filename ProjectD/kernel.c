void printString(char*);
void readString(char*);
int mod(int, int);
int div(int, int);
void readSector(char*, int);
void writeSector(char*, int);
void handleInterrupt21(int, int, int, int);
void readFile(char*, char*);
void executeProgram(char*, int);
void terminate();
void deleteFile(char*);
void writeFile(char*, char*, int);
void copyFile(char*, char*);
void lsCmd();
void createFile(char*);
int main(){
	makeInterrupt21();
	interrupt(0x10, 0x2*256, 0, 0, 0);
	interrupt(0x10, 0x9*256+'\0', 0*256+0xa, 10000, 0);
	interrupt(0x21, 4, "shell\0", 0x2000, 0);
	while(1){
		
	}
	return 0;
}

void printString(char* chars){
	int i=0;
	int k=0;
	while(chars[i] != '\0'){
		interrupt(0x10, 0xe*256+chars[i], 0, 0, 0);
		i++;
		if(chars[i-1] == '\n'){
			for(k=0; k<i; k++){
				interrupt(0x10, 0xe*256+0x8, 0, 0, 0);
			}
		}
	}
}

void readString(char* chars){
	int i=0;
	chars[i] = interrupt(0x16, 0, 0, 0, 0);
	if(chars[i] == 0x8){
		return;
	}
	interrupt(0x10, 0xe*256+chars[i], 0, 0, 0);
	i++;
	while(chars[i-1] != 0x0d){
		chars[i] = interrupt(0x16, 0, 0, 0, 0);
		if(chars[i] == 0x8){
			if(i>0){
				i = i-1;
				interrupt(0x10, 0xe*256+0x8, 0, 0, 0);
				interrupt(0x10, 0xe*256+'\0', 0, 0, 0);
				interrupt(0x10, 0xe*256+0x8, 0, 0, 0);
			}
		}
		else{
			interrupt(0x10, 0xe*256+chars[i], 0, 0, 0);
			i++;
		}
	}
	chars[i] = 0xa;
	chars[i+1] = 0xd;
	interrupt(0x10, 0xe*256+chars[i], 0, 0, 0);
	interrupt(0x10, 0xe*256+chars[i+1], 0, 0, 0);
	chars[i+2] = '\0';
}


int mod(int a, int b){
	while(a>=b){
		a = a - b;
	}
	return a;
}

int div(int a, int b){
	int q = 0;
	while((q * b) < a){
		q = q + 1;
	}
	q=q-1;
	return q;
}

void readSector(char* buffer, int sector){
	int sec=sector;
	char* buf=buffer;
	int modCl=mod(sec, 18);
	int divDh=div(sec, 18);
	int cl=modCl+1;
	int dh=mod(divDh, 2);
	int ch=div(sec, 36);
	int cx=ch*256+cl;
	int dx=dh*256;
	int ax=2*256+1;
	interrupt(0x13, ax, buffer, cx, dx);
}

void writeSector(char* buffer, int sector){
	int sec=sector;
	char* buf=buffer;
	int modCl=mod(sec, 18);
	int divDh=div(sec, 18);
	int cl=modCl+1;
	int dh=mod(divDh, 2);
	int ch=div(sec, 36);
	int cx=ch*256+cl;
	int dx=dh*256;
	int ax=3*256+1;
	interrupt(0x13, ax, buffer, cx, dx);
}

void handleInterrupt21(int ax, int bx, int cx, int dx){
	if(ax>=11){
		printString("Error: Functionality not defined yet.\n\0");
	}	
	if(ax==0){
		printString(bx);
	}
	if(ax==1){	
		readString(bx);
	}
	if(ax==2){
		readSector(bx, cx);
		printString(bx);
	}
	if(ax==3){
		readFile(bx, cx);
	}
	if(ax==4){
		executeProgram(bx, cx);
	}
	if(ax==5){
		terminate();
	}
	if(ax==6){
		writeSector(bx, cx);
	}
	if(ax==7){
		deleteFile(bx);
	}
	if(ax==8){
		copyFile(bx, cx);
	}
	if(ax==9){
		lsCmd();
	}
	if(ax==10){
		createFile(bx);
	}
}
	
void readFile(char* filename, char* buffer){
	char sector[512];
	char filesector[26];
	int i;
	int j = 0;
	int k;
	int match = 0;
	readSector(sector, 2);
	for(i = 0; i < 512; i+=32) {
		if(*(sector+i) != 0x00){
			for(j=0; j<6; j++){
				if((sector[i] != 0x00) && (filename[j] == sector[i+j] || sector[i+j] == '\0')){
					match++;
					if(match==6){
						for(k=6; k<32; k++){
							filesector[k-6] = sector[i+k];
						}
						for(k=0; k<26; k++){
							if(filesector[k] != 0x00){
								readSector(buffer+k*512, filesector[k]);
							}
						}
						return;
					}
				}
				else{
					match=0;
				}
			}
		}	
	}
	return;
}

void executeProgram(char* name, int segment){
	char buffer[13312];
	int i;
	int pos;
	int modSeg;
	pos = 0;
	modSeg = mod(segment, 0x1000);
	if(segment == 0x0000 || segment >= 0xA000 || modSeg != 0 || segment == 0x1000){
		return;
	}
	readFile(name, buffer);
	for(i = 0; i<13312; i++){
		putInMemory(segment, i, buffer[i]);
	}
	launchProgram(segment);
}

void terminate(){
	char shell[6];
	*(shell+0) = 's';
	*(shell+1) = 'h';
	*(shell+2) = 'e';
	*(shell+3) = 'l';
	*(shell+4) = 'l';
	*(shell+5) = '\0';
	
	interrupt(0x21, 4, shell, 0x2000, 0);
}

void deleteFile(char* name){
	char dir[512];
	char map[512];
	int i;
	int j;
	int k=0;
	int match=0;
	char filesector[26];
	readSector(dir, 2);
	readSector(map, 1);
	
	for(i = 0; i < 512; i+=32) {
		for(j=0; j<6; j++){
			if(name[j] == dir[i+j]){
				match++;
				if(match==6){
					*(dir+i)=0x0;
					for(k=6; k<32; k++){
						filesector[k-6] = dir[i+k];
					}
					for(k=0; k<26; k++){
						if(filesector[k] != 0x0){
							int sector = *(filesector+k);
							*(map+sector) = 0x0;
						}
					}
					writeSector(dir, 2);
					writeSector(map, 1);
				}
			}
			else{
				match=0;
			}
		}
	}
}

void writeFile(char* name, char* buffer, int numberOfSectors){
	char dir[512];
	char map[512];
	int i;
	int j=0;
	int k;
	int count=0;
	int match=0;
	int pos=6;
	int found=0;
	int tempBuffer[512];
	char filesector[26];
	
	readSector(dir, 2);
	readSector(map, 1);
	
	
	for(i=0; i<512; i+=32){
		if(dir[i]==0x00){
			match=i;
			while(name[j] != '\0'){
				dir[i+j] = name[j];
				count++;
				j++;
			}
			found=1;
			break;
		}
	}
	
	if(found==0){
		return;
	}
	found=0;
	
	if(count<6){
		for(i=count; i<32; i++){
			dir[i] = 0x00;
		}
	}
	
	for(k=0; k<numberOfSectors; k++){
		if(found==1){
			break;
		}
		for(j=0; j<512; j++){
			if(*(map+j)==0x0){
				found=1;
				*(map+j) = 0xFF;
				*(dir+match+pos) = j;
				pos++;
				writeSector(buffer+k*512, j);
				break;
			}
		}
	}
	
	if(found==0){
		return;
	}
	
	writeSector(dir, 2);
	writeSector(map, 1);
}

void copyFile(char* file1, char* file2) {
	char buffer[13312];
	char buf[13312];
	char dir[512];
	int match=0;
	int i;
	int j;
	int k;
	int numSectors=0;
	int flag=0;
	readSector(dir, 2);
	
	for(i = 0; i < 512; i+=32) {
		if(flag==1){
			break;
		}
		for(j=0; j<6; j++){
			if(file1[j] == dir[i+j]){
				match++;
				if(match==6){
					for(k=6; k<32; k++){
						if(dir[i+k] != 0x00){
							numSectors++;
						}
					}
					flag=1;
					break;
				}
			}
			else{
				match=0;
			}
		}	
	}
	
	readFile(file1, buffer);
	
	writeFile(file2, buffer, numSectors);
}

void lsCmd(){
	int i;
	int j;
	char dir[512];
	char name[7];
	char newline[2];
	*(newline+0)='\n';
	*(newline+1)='\0';
	readSector(dir, 2);
	
	for(i=0; i<512; i+=32){
		if(*(dir+i) != 0x00){
			for(j=0; j<6; j++){
				if(*(dir+i+j) != '\0'){
					*(name+j) = *(dir+i+j);
				}
			}
			*(name+j)='\n';
			*(name+j+1)='\0';
			printString(name);			
		}
	}
}

void createFile(char* filename){
	char buffer[13312];
	char line[80];
	int loop=1;
	int i=0;
	int count=0;
	int bufpos=0;
	int numSectors=0;
	while(loop){
		count=0;
		readString(line);
		
		for(i=0; i<80; i++){
			if(line[i] != '\0'){
				count++;
			}
		}
		
		if((count-3)==0){
			loop=0;
			break;
		}
		
		for(i=0; i<80; i++){
			if(line[i] != '\0'){
				buffer[bufpos] = line[i];
				bufpos++;
				line[i]='\0';
			}
		}
	}
	buffer[bufpos]='\0';
	numSectors = div(bufpos, 512) + 1;
	writeFile(filename, buffer, numSectors);
	
}
