#include<iostream>
#include<fstream>
#include<cstring>
#include<string>
#include<map>

#define byte unsigned char

using namespace std;

char charsets[64]={0};
char rcharsets[127] = {0};

void initial_charsets(){
	int i,j;
	while(i < 62){
		if(i < 26){
			charsets[i] = 'A' + i;
		}
		else if(i < 52){
			charsets[i] = 'a' + (i - 26);
		}
		else{
			charsets[i] = '0' + (i - 52);
		}
		i ++;
	}
	charsets[62] = '*';
	charsets[63] = '/';

	i = 0;
	while(i < 64){
		rcharsets[charsets[i]] = i;
		i++;
	}
}

// encode byte stream with base64
// 使用len而不是直接使用bytes的长度，考虑到编码文件中包含'\0'的情况
string encode(const byte bytes[], int len){
    
    int i = 0,j = 0;
    int rlen = len / 3 * 4 + (len % 3 != 0) * 4;

    string result = "";

	char temps[5] = {0};    
    // encode with every three char
    while(i + 3 <= len){

    	temps[0] = charsets[ bytes[i] >> 2 ];
    	temps[1] = charsets[ ((bytes[i] & 0x03) << 4 ) + (bytes[i + 1] >> 4)];
    	temps[2] = charsets[ ((bytes[i + 1] & 0x0f) << 2 ) + (bytes[i + 2] >> 6) ];
    	temps[3] = charsets[ ((bytes[i + 2]) & 0x3f) ];
    	temps[4] = 0;

    	result += string(temps);
    	i += 3;
    }

    // append '='
    if(len % 3 == 2){

    	temps[0] = charsets[ bytes[i] >> 2 ];
    	temps[1] = charsets[ ((bytes[i] & 0x03) << 4 ) + (bytes[i + 1] >> 4)];
    	temps[2] = charsets[ ((bytes[i + 1] & 0x0f) << 2 ) ];
    	temps[3] = '=';
    	temps[4] = 0;
    	result += string(temps);

    }else if(len % 3 == 1){

    	temps[0] = charsets[ bytes[i] >> 2 ];
    	temps[1] = charsets[ ((bytes[i] & 0x03) << 4 )];
    	temps[2] = '=';
    	temps[3] = '=';
    	temps[4] = 0;

    	result += string(temps);
    }

    return result;
}


// decode base64 string into byte stream
byte* decode(string str, int len){

	if(len % 4 != 0){
		cout << "Illegal base64 string with length of" << len << endl;
		return NULL;
	}

	int rlen = len / 4 * 3;
	byte* bytes = new byte [rlen + 1];
	bytes[rlen] = 0;

	int i = 0, j = 0;
	while( i + 4 <= len){

		bytes[j] = (rcharsets[str[i]] << 2 ) + ((rcharsets[str[i + 1]] & 0x3f) >> 4);
		bytes[j + 1] = ((rcharsets[str[i + 1]] & 0x0f) << 4) + ((rcharsets[str[i + 2]] >> 2) & 0x0f);
		bytes[j + 2] = ((rcharsets[str[i + 2]] & 0x03) << 6 ) + ( rcharsets[str[i + 3]]);

		i += 4;
		j += 3;
	}

	return bytes;
}


void help(){
	cout << "Command [\'base64\'] Usage:" << endl;
	cout << "\t -h : Display this message." << endl;
	cout << "\t -e : Encode byte stream with base64." << endl;
	cout << "\t -d : Decode base64 string into bytes stream." << endl;
}

int main(int argc, char const *argv[])
{

	initial_charsets();
	
	if(argc == 1){
		help();
		return 0;
	}else if(argc % 2 == 0){
		cout << "Invalid input" << endl;
		return 0;
	}


	// get parameters
	std::map<string, string> params;
	int i = 1;
	while(i < argc){
		pair<string, string> p(string(argv[i]), string(argv[i + 1]));
		params.insert(p);
		i += 2;
	}

	// parse all the recieved paramters
	for(std::map<string, string>::iterator temp = params.begin(); temp != params.end(); temp ++){
		string option = temp->first;
		string value = temp->second;

		if(option == "-e"){
			cout << encode((byte*)value.c_str(), value.size()) << endl;
		}else if( option == "-d"){
			cout << decode(value, value.size()) << endl;
		}else if( option == "-ef"){

			// encode files

			string output_file = params["-o"];
			string input_file = params["-i"];

			ifstream inf(input_file.c_str(), ios_base::binary);
			ofstream outf(output_file.c_str(), ios_base::binary);

			int bufferlen = 300;
			char* buffer = new char[bufferlen];

			while(inf.read(buffer, bufferlen)){
				outf << encode((byte*)buffer, bufferlen);
			}

			// 读到此处截断
			buffer[inf.gcount()] = 0;
			outf << encode((byte*)buffer, inf.gcount());

			inf.close();
			outf.close();

			delete [] buffer;
			buffer = NULL;

		}else if( option == "-df"){

			string output_file = params["-o"];
			string input_file = params["-i"];

			ifstream inf(input_file.c_str(), ios_base::binary);
			ofstream outf(output_file.c_str(), ios_base::binary);

			int bufferlen = 400;
			char* buffer = new char[bufferlen];
           
            int read_count = 0;
            while(inf.read(buffer, bufferlen)){
            	// decode输出是bufferlen * 3 /4 
                outf.write((char*) decode(string(buffer), bufferlen), bufferlen * 3 / 4 );
            }

            outf.write((char*) decode(string(buffer), inf.gcount()) , inf.gcount() * 3 / 4);

			inf.close();
			outf.close();

			delete [] buffer;
			buffer = NULL;
		}

	}

	return 0;
}