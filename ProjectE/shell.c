void splitInput(char*, char*, char*, char*);
void cmd1(char*, int);
void cmd2(char*, char*, int);
void cmd3(char*, char*, char*, int);
void typeCmd(char*);
void printError();
void chclrCmd(char*);
void clearScreen();
int code=0xa;
int main(){
	char line[80];
	char command[80];
	char arguement[80];
	char arg2[80];
	int check=0;
	enableInterrupts();
	while(1){
		if(check==0){
			interrupt(0x21, 0, "SHELL>\0", 0, 0);
			interrupt(0x21, 1, line, 0, 0);
			if(line[0]!=0x8){
				splitInput(line, command, arguement, arg2);
				check=0;
			}
			else{
				check=1;
			}			
			
		}
		else{
			interrupt(0x21, 1, line, 0, 0);
			if(line[0]!=0x8){
				splitInput(line, command, arguement, arg2);
				check=0;
			}
			else{
				check=1;
			}
		}
	}
	return 0;
}
void splitInput(char* line, char* command, char* arguement, char* arg2){
	int i=0;
	int pos = 0;
	int pos2=0;
	int end = 0;
	int len2 = 0;
	int len1 = 0;
	
	while(line[end]!='\0'){ 
		end++;
	}
	
	while(line[pos]!=0x20){ 
		pos++;
		if(pos>=end){
			for(i=0; i<(end); i++){
				command[i] = line[i];
			}
			len1=end-3;
			cmd1(command, len1);
			return;
		}
	}
	
	pos2 = pos+1;
	
	while(line[pos2]!=0x20){ 
		pos2++;
		if(pos2>=end){
			for(i=0; i<pos; i++){
				command[i] = line[i];
			}
			len1 = pos;
			for(i=pos+1; i<end; i++){
				arguement[len2] = line[i];
				len2++;
			}
			arguement[len2]='\0';
			cmd2(command, arguement, len1);
			return;
		}
	}
	
	for(i=0; i<pos; i++){
		command[i] = line[i];
	}
	len1 = pos;
	
	for(i=pos+1; i<pos2; i++){
		arguement[len2] = line[i];
		len2++;
	}
	
	arguement[len2]='\0';
	
	len2=0;
	
	for(i=pos2+1; i<(end-3); i++){
		arg2[len2] = line[i];
		len2++;
	}
	arg2[len2]='\0';
	
	cmd3(command, arguement, arg2, len1);
}

void cmd1(char* command, int len1){
	
	if(len1==0){
		if(command[0]==0x03){
			return;
		}
	}
	
	if(len1==1){
		if(command[0]=='0'){
			interrupt(0x21, 11, 0, 0, 0);
			return;
		}
		if(command[0]=='1'){
			interrupt(0x21, 11, 1, 0, 0);
			return;
		}
		if(command[0]=='2'){
			interrupt(0x21, 11, 2, 0, 0);
			return;
		}
		if(command[0]=='3'){
			interrupt(0x21, 11, 3, 0, 0);
			return;
		}
		if(command[0]=='4'){
			interrupt(0x21, 11, 4, 0, 0);
			return;
		}
		if(command[0]=='5'){
			interrupt(0x21, 11, 5, 0, 0);
			return;
		}
		if(command[0]=='6'){
			interrupt(0x21, 11, 6, 0, 0);
			return;
		}
		if(command[0]=='7'){
			interrupt(0x21, 11, 7, 0, 0);
			return;
		}
	}
	
	if(len1==2){
		if(command[0]=='l' && command[1]=='s'){
			interrupt(0x21, 9, 0, 0, 0);
			return;
		}
	}
	
	if(len1==3){
		if(command[0]=='d' && command[1]=='i' && command[2]=='r'){
			interrupt(0x21, 9, 0, 0, 0);
			return;
		}
	}
	
	if(len1==4){
		if(command[0]=='h' && command[1]=='e' && command[2]=='l' && command[3]=='p'){
			typeCmd("helplt\0");
			return;
		}
	}
	
	if(len1==5){
		if(command[0]=='c' && command[1]=='l' && command[2]=='e' && command[3]=='a' && command[4]=='r'){
			clearScreen();	
			return;
		}
	}
	
	printError();
	return;
}

void cmd2(char* command, char* arguement, int len1){
	
	if(len1==4){
		if(command[0]=='t' && command[1]=='y' && command[2]=='p' && command[3]=='e'){
			typeCmd(arguement);
			return;
		}
		
		if(command[0]=='k' && command[1]=='i' && command[2]=='l' && command[3]=='l'){
			interrupt(0x21, 11, arguement, 0, 0);
			return;
		}
	}
	
	if(len1==5){
		if(command[0]=='c' && command[1]=='h' && command[2]=='c' && command[3]=='l' && command[4]=='r'){
			chclrCmd(arguement);
			return;
		}
	}
	
	if(len1==6){
		if(command[0]=='d' && command[1]=='e' && command[2]=='l' && command[3]=='e' && command[4]=='t' && command[5]=='e'){
			interrupt(0x21, 7, arguement, 0, 0);
			return;
		}
		if(command[0]=='c' && command[1]=='r' && command[2]=='e' && command[3]=='a' && command[4]=='t' && command[5]=='e'){
			interrupt(0x21, 10, arguement, 0, 0);
			return;
		}
	}
	
	if(len1==7){
		if(command[0]=='e' && command[1]=='x' && command[2]=='e' && command[3]=='c' && command[4]=='u' && command[5]=='t' && command[6]=='e'){
			interrupt(0x21, 4, arguement, 0, 0);
			return;
		}
	}
	printError();
	return;
}

void cmd3(char* command, char* file1, char* file2, int len1){
	
	if(len1==4){
		if(command[0]=='c' && command[1]=='o' && command[2]=='p' && command[3]=='y'){
			interrupt(0x21, 8, file1, file2, 0);
			return;
		}
	}
	
	printError();
	return;
}

void typeCmd(char* arguement){
	char buffer[23312];
	int i=0;
	*(buffer+0)='\0';
	interrupt(0x21, 3, arguement, buffer, 0);
	if(buffer[0] != '\0'){
		interrupt(0x21, 0, buffer, 0, 0);
	}
}

void printError(){
	char error[7];
	*(error+0) = 'E';
	*(error+1) = 'r';
	*(error+2) = 'r';
	*(error+3) = 'o';
	*(error+4) = 'r';
	*(error+5) = '\n';
	*(error+6) = '\0';
	interrupt(0x21, 0, error, 0, 0);
}

void chclrCmd(char* arguement){
	if(arguement[0] >= 48 && arguement[0] <=57){
		code = arguement[0]-48;
	}
	if (arguement[0] >= 65 && arguement[0] <=70){
		code = arguement[0]-55;
	}
	if (arguement[0] >= 97 && arguement[0] <=102){
		code = arguement[0]-87;
	}
	interrupt(0x10, 0x2*256, 0, 0, 0);
	interrupt(0x10, 0x9*256+'\0', 0*256+code, 2000, 0);
	interrupt(0x10, 0x2*256, 0, 0, 0);
}

void clearScreen(){
	interrupt(0x10, 0x2*256, 0, 0, 0);
	interrupt(0x10, 0x9*256+'\0', 0*256+code, 2000, 0);
	interrupt(0x10, 0x2*256, 0, 0, 0);
}
