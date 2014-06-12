void printString(char*);
void readString(char*);
int mod(int, int);
int div(int, int);
void readSector(char*, int);
void handleInterrupt21(int, int, int, int);
void readFile(char*, char*);
void executeProgram(char*, int);
void terminate();
int main(){
	char buffer[13312];
	makeInterrupt21();
	/*interrupt(0x21, 3, "messag\0", buffer, 0);
	interrupt(0x21, 0, buffer, 0, 0);*/
	interrupt(0x21, 4, "shell\0", 0x2000, 0);
	while(1){
		
	}
	return 0;
}

void printString(char* chars){
	int i=0;
	int count=0;
	int k=0;
	while(chars[i] != '\0'){
		if(chars[i]=='\n'){
			interrupt(0x10, 0xe*256+chars[i], 0, 0, 0);
			i++;
			count++;
			for(k=0; k<count; k++){
				interrupt(0x10, 0xe*256+0x8, 0, 0, 0);
			}
		}else{
			interrupt(0x10, 0xe*256+chars[i], 0, 0, 0);
			i++;
			count++;
		}
	}
}

void readString(char* chars){
	int i=0;
	chars[i] = interrupt(0x16, 0, 0, 0, 0);
	
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

void handleInterrupt21(int ax, int bx, int cx, int dx){
	if(ax>=6){
		printString("Error: Functionality not defined yet. \0");
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
}
	
void readFile(char* filename, char* buffer){
	char sector[512];
	char filesector[26];
	char filechunk[512];
	int i;
	int j = 0;
	int k;
	int match = 0;
	int pos = 0;
	readSector(sector, 2);
	for(i = 0; i < 512; i+=32) {
		for(j=0; j<6; j++){
			if(filename[j] == sector[i+j]){
				match++;
				if(match==6){
					for(k=6; k<32; k++){
						filesector[k-6] = sector[i+k];
					}
					for(k=0; k<26; k++){
						if(filesector[k] != 0){
							readSector(buffer+(k*512), filesector[k]);
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
