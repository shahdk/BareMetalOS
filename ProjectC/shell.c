/*The shell code*/
void splitInput(char*, char*, char*);
int main(){
	char line[80];
	char command[80];
	char arguement[80];
	while(1){
		if(check==0){
			interrupt(0x21, 0, "SHELL>\0", 0, 0);
			interrupt(0x21, 1, line, 0, 0);
			if(line[0]!=0x8){
				splitInput(line, command, arguement);
				check=0;
			}
			else{
				check=1;
			}
		}
		else{
			interrupt(0x21, 1, line, 0, 0);
			if(line[0]!=0x8){
				splitInput(line, command, arguement);
				check=0;
			}
			else{
				check=1;
			}
		}
	}
	return 0;
}
void splitInput(char* line, char* command, char* arguement){
	int i=0;
	int pos = 0;
	int end = 0;
	int len2 = 0;
	int len1 = 0;
	char buffer[13312];
	while(line[end]!='\0'){ 
		end++;
	}
	while(line[pos]!=0x20){ 
		pos++;
		if(pos>=end){
			interrupt(0x21, 0, "Bad Command\n\0", 0, 0);
			return;
		}
	}
	for(i=0; i<pos; i++){
		command[i] = line[i];
	}
	len1 = pos;
	for(i=pos+1; i<end; i++){
		arguement[len2] = line[i];
		len2++;
	}
	arguement[len2]='\0';
	if(len1 != 4 && len1 != 7 && len1 != 5){
		interrupt(0x21, 0, "Bad Command\n\0", 0, 0);
		return;
	}
	if(len1==4){
		if(command[0]=='t' && command[1]=='y' && command[2]=='p' && command[3]=='e'){
			interrupt(0x21, 3, arguement, buffer, 0);
			interrupt(0x21, 0, buffer, 0, 0);
		}
	}
	
	if(len1==5){
		if(command[0]=='c' && command[1]=='l' && command[2]=='e' && command[3]=='a' && command[4]=='r'){
			if(arguement[0]=='s' && arguement[1]=='c' && arguement[2]=='r' && arguement[3]=='e' && arguement[4]=='e' && arguement[5]=='n'){
				interrupt(0x10, 0x2*256, 0, 0, 0);
				interrupt(0x10, 0x9*256+'\0', 0*256+0xf, 10000, 0);
				interrupt(0x10, 0x2*256, 0, 0, 0);
				return;
			}
		}
	}
	
	if(len1==7){
		if(command[0]=='e' && command[1]=='x' && command[2]=='e' && command[3]=='c' && command[4]=='u' && command[5]=='t' && command[6]=='e'){
			interrupt(0x21, 4, arguement, 0x2000, 0);
		}
	}
}

