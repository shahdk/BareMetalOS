//Dharmin Shah, Rain Dartt, Rob Wagner
void printString(char*);
void readString(char*);
int mod(int, int);
int div(int, int);
void readSector(char*, int);
void handleInterrupt21(int, int, int, int);	
int main(){
	char line[80];
	char chars[80];
	char buffer[512];
	char color[80];
	printString("Enter line: \0");
	readString(line);
	printString(line);
	readSector(buffer, 30);
	printString(buffer);
	makeInterrupt21();
	printString("Enter line for interrupt 21: \0");
	interrupt(0x21,1,chars,0,0);
	interrupt(0x21,0,chars,0,0);
	interrupt(0x21,2,buffer,30,0);
	while(1){}
}

void printString(char* chars){
	int i=0;
	while(chars[i] != 0x0){
		interrupt(0x10, 0xe*256+chars[i], 0, 0, 0);
		i++;
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
			if(i>=0){
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
	chars[i] = '\n';
	interrupt(0x10, 0xe*256+chars[i], 0, 0, 0);
	chars[i+1] = '\0';
}


int mod(int a, int b){
	int x;
	int y;
	x=a;
	y=b;
	while(x>=y){
		x = x-y;
	}
	return x;
}

int div(int a, int b){
	int x=a;
	int y=b;
	int q=0;
	while((q*y)<x){
		q=q+1;
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
	if(ax>=3){
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
	
}
	
