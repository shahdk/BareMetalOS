void printString(char*);
void readString(char*);
int mod(int, int);
int div(int, int);
void readSector(char*, int);
void writeSector(char*, int);
void handleInterrupt21(int, int, int, int);
void readFile(char*, char*);
void executeProgram(char*);
void terminate();
void deleteFile(char*);
void writeFile(char*, char*, int);
void copyFile(char*, char*);
void lsCmd();
void createFile(char*);
void handleTimerInterrupt(int, int);
void kill(int);
typedef struct{
	int active;
	int sp;
}Process;

Process pTable[8];
int currentProcess;
int chk;

int main(){
	int i;
	i=0;
	currentProcess=0;
	chk=0;
	for(i=0; i<8; i++){
		Process temp;
		temp.active = 0;
		temp.sp = 0xFF00;
		pTable[i] = temp;
		
	}
	makeInterrupt21();
	makeTimerInterrupt();
	interrupt(0x10, 0x2*256, 0, 0, 0);
	interrupt(0x10, 0x9*256+'\0', 0*256+0xa, 2000, 0);
	executeProgram("shell\0");
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
	int k=0;
	chars[i] = interrupt(0x16, 0, 0, 0, 0);
	if(chars[i] == 0x8){
		return;
	}
	if(chars[0] == 0x3){
		setKernelDataSegment();
		if(currentProcess+1 < 8){
			interrupt(0x21, 11, currentProcess+1, 0, 0);
		}
		restoreDataSegment();
		chars[1] = 0xa;
		chars[2] = 0xd;
		interrupt(0x10, 0xe*256+chars[1], 0, 0, 0);
		interrupt(0x10, 0xe*256+chars[2], 0, 0, 0);
		chars[3] = '\0';
		return;
	}
	interrupt(0x10, 0xe*256+chars[i], 0, 0, 0);
	i++;
	while(chars[i-1] != 0x0d){
		chars[i] = interrupt(0x16, 0, 0, 0, 0);
		if(chars[i] == 0x9 && i==2){
			if(chars[0] == 't' && chars[1] == 'y'){
				chars[2] = 'p';
				chars[3] = 'e';
				i=4;
			}
			if(chars[0] == 'h' && chars[1] == 'e'){
				chars[2] = 'l';
				chars[3] = 'p';
				i=4;
			}
			if(chars[0] == 'd' && chars[1] == 'e'){
				chars[2] = 'l';
				chars[3] = 'e';
				chars[4] = 't';
				chars[5] = 'e';
				i=6;
			}
			if(chars[0] == 'c' && chars[1] == 'o'){
				chars[2] = 'p';
				chars[3] = 'y';
				i=4;
			}
			if(chars[0] == 'c' && chars[1] == 'r'){
				chars[2] = 'e';
				chars[3] = 'a';
				chars[4] = 't';
				chars[5] = 'e';
				i=6;
			}
			if(chars[0] == 'e' && chars[1] == 'x'){
				chars[2] = 'e';
				chars[3] = 'c';
				chars[4] = 'u';
				chars[5] = 't';
				chars[6] = 'e';
				i=7;
			}
			if(chars[0] == 'c' && chars[1] == 'h'){
				chars[2] = 'c';
				chars[3] = 'l';
				chars[4] = 'r';
				i=5;				
			}
			if(chars[0] == 'c' && chars[1] == 'l'){
				chars[2] = 'e';
				chars[3] = 'a';
				chars[4] = 'r';
				i=5;
			}
			if(chars[0] == 'k' && chars[1] == 'i'){
				chars[2] = 'l';
				chars[3] = 'l';
				i=4;
			}
			for(k=2; k<i; k++){
				interrupt(0x10, 0xe*256+chars[k], 0, 0, 0);
			}
		}
		else if(chars[i] == 0x8){
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
	char error[7];
	*(error+0) = 'E';
	*(error+1) = 'r';
	*(error+2) = 'r';
	*(error+3) = 'o';
	*(error+4) = 'r';
	*(error+5) = '\n';
	*(error+6) = '\0';
	if(ax>=12){
		printString(error);
	}	
	if(ax==0){
		printString((char *)bx);
	}
	if(ax==1){	
		readString((char *)bx);
	}
	if(ax==2){
		readSector((char *)bx, cx);
		printString((char *)bx);
	}
	if(ax==3){
		readFile((char *)bx, (char *)cx);
	}
	if(ax==4){
		executeProgram((char *)bx);
	}
	if(ax==5){
		terminate();
	}
	if(ax==6){
		writeSector((char *)bx, cx);
	}
	if(ax==7){
		deleteFile((char *)bx);
	}
	if(ax==8){
		copyFile((char *)bx, (char *)cx);
	}
	if(ax==9){
		lsCmd();
	}
	if(ax==10){
		createFile((char *)bx);
	}
	if(ax==11){
		kill(bx);
	}
}
	
void terminate(){
	setKernelDataSegment();
	pTable[currentProcess].active = 0;
	while(1) {}
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
	}
	return;
}
	
void executeProgram(char* name){
	char buffer[13312];
	int i;
	int j;
	int p;
	int segment;
	
	chk=1;
	
	readFile(name, buffer);
	
	for(i=0; i<8; i++){
		setKernelDataSegment();
		if(pTable[i].active==0){
			pTable[i].sp=0xFF00;
			p=i;
			restoreDataSegment();
			segment = (i+2)*0x1000;
			break;
		}
		restoreDataSegment();
	}
		
	for(j = 0; j<13312; j++){
		putInMemory(segment, j, buffer[j]);
	}
	
	initializeProgram(segment);	
    setKernelDataSegment();	
	pTable[p].active = 1;
	restoreDataSegment();
	chk=0;
}



void handleTimerInterrupt(int segment, int sp){
	int i;
	int j;
	int k;
	int origProcess;
	int stackP;
	int seg;
	
	stackP = sp;
	seg=segment;
	
	if(chk==1){
		returnFromTimer(segment, sp);
		return;
	}
	
	if(segment == 0x1000 * (currentProcess+2)){
		pTable[currentProcess].sp = sp;
	}
	
	for(i=0;i<8;i++){
		j = currentProcess+1;
		j = mod(j+i,8);

		if(pTable[j].active == 1){
			seg = (j+2)*0x1000;
			stackP = pTable[j].sp;
			currentProcess = j;
			break;
		}
	}	

	returnFromTimer(seg,stackP);
	return;
}

void kill(int process){
	setKernelDataSegment();
	chk = 1;
	pTable[process].active = 0;
	chk=0;
	restoreDataSegment();
	return;
}
