#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>
#include <map>
#include <ctime>
using namespace std;
vector<string> Command, SLable;
map<string, int> Lable, Order, Register, LableAddress;
int DataPos[100], Reg[32], Dnum = 0;
int Mpos, TotalLine, Flag = 0;
int Hpptr = 0, PC = 0, HI = 0, LO = 0;
char Memory[4*1024*1024];
int Parameter[5] = {0}, ETOM[4] = {0}, MTOW[4] = {0};
bool RegNeeded[34] = {0};

//Tools used for Pipeline:

class AllocateMemory{
public:
    void IntToByte(char ans[],int data[],int NumOfInt = 1,int Type = 0){
        int Pos = 0;
        for(int i = 0;i < NumOfInt;i++){
            ans[Pos++] = (data[i]) >> 24;
            ans[Pos++] = (data[i]) >> 16;
            ans[Pos++] = (data[i]) >> 8;
            ans[Pos++] = data[i];
        }
    }

    int ByteToInt(char data[]){
        int Pos = 0,A,B,C,D;
        int c = (1<<8) - 1;
        A = ((int)data[Pos++])&c;
        B = ((int)data[Pos++])&c;
        C = ((int)data[Pos++])&c;
        D = ((int)data[Pos++])&c;
        int ans = (A << 24) + (B << 16) + (C << 8) + D;
        return ans;
    }

    void HalfToByte(char ans[],int data[],int NumOfHalf = 1,int Type = 0){
        int Pos = 0;
        for(int i = 0;i < NumOfHalf;i++){
            ans[Pos++] = ((short)(data[i])) >> 8;
            ans[Pos++] = (short)data[i];
        }
    }

    int ByteToHalf(char data[]){
        int Pos = 0,A,B;
        short c = (1<<8) - 1;
        A = ((short)data[Pos++])&c;
        B = ((short)data[Pos++])&c;
        short ans = (A << 8) + B;
        return ans;
    }

    void AllocateByte(int data[],int Pos,int NumOfByte = 1){
        for(int i = 0;i < NumOfByte;i++)
            Memory[Pos + i] = data[i];
    }

    void AllocateInt(int data[],int Pos,int NumOfInt = 1){
        char *tem = new char[NumOfInt*4];
        IntToByte(tem,data,NumOfInt);
        int r = 0;
        for(int i = Pos;i < Pos + NumOfInt*4;i++)
            Memory[i] = tem[r++];
    }

    void AllocateHalf(int data[],int Pos,int NumOfHalf = 1){
        char *tem = new char[NumOfHalf*2];
        HalfToByte(tem,data,NumOfHalf);
        int r = 0;
        for(int i = Pos;i < Pos + NumOfHalf*2;i++)
            Memory[i] = tem[r++];
    }

    void AllocateString(string str, int Pos){
        int NumOfChar = str.length();
        for(int i = 0;i < NumOfChar;i++)
            Memory[Pos + i] = str[i];
    }

    int GetIntFromMemory(int Pos,int Type){
        if(Type == 1){                  //Half
            char *tem = new char[2];
            tem[0] = Memory[Pos];
            tem[1] = Memory[Pos + 1];
            return ByteToHalf(tem);
        }
        else if(Type == 2){             //Int
            char *tem = new char[4];
            for(int i = 0;i < 4;i++)
                tem[i] = Memory[Pos + i];
            return ByteToInt(tem);
        }
    }

    void AllocateEmpty(int n){
        Hpptr += n;
    }

    void Align(int n){
        int width = 1 << n;
        if(Hpptr % width == 0)return;
        else Hpptr = (Hpptr/width + 1) << n;
    }

    void Load(int data[],int operation){   //type + register + address
        if(operation == 50) data[2] = Memory[data[2]];
        else if(operation == 51) data[2] = GetIntFromMemory(data[2],1);             //HalfToInt
        else if(operation == 52) data[2] = GetIntFromMemory(data[2],2);
    }

    void Store(int data[],int operation){   //type + value + address
        int *Tem = new int[1];
        Tem[0] = data[1];
        if(operation == 53)	Memory[data[2]] = data[1];
        else if(operation == 54) AllocateHalf(Tem, data[2]);
        else if(operation == 55) AllocateInt(Tem, data[2]);
    }

};

class DivideCode{
public:
    bool IsInt(char ch){
        return ((0 <= ch - '0' && ch - '0' <= 9) || ch == '-');
    }

    bool IfJump(char ch){
        return (ch == ',' || ch == ' ');
    }

    int StringToInt(string str){
        int r = 0,ans = 0,minor = 0;
        if(str[r] == '-'){
            minor = 1;
            r++;
        }
        while( IsInt(str[r]) )
            ans = ans*10 + str[r++] - '0';
        if(minor) ans *= -1;
        return ans;
    }

    int DivOrderType(string str,string &ans){
        int CutPos = 0;
        string CuttedLable;
        while(CutPos < str.length() && str[CutPos] != ' ')
            CutPos++;

        if(CutPos == str.length()){
            CuttedLable = str;
            ans = "";
        }
        else{
            ans = str.substr(CutPos + 1,str.length() - CutPos + 1);
            CuttedLable = str.substr(0,CutPos);
        }
        if(CuttedLable == "" && ans == "")return 0;
        return Order[CuttedLable];
    }

    void DivAsciiz(string str,string &ans){
        int r = 0,l = 0;
        ans = "";
        str = str.substr(1,str.length() - 2);
        for(int i = 0;i < str.length();i++){
            if(str[i] == '\\'){
                if(str[i+1] == 'n')ans += '\n';
                if(str[i+1] == 't')ans += '\t';
                if(str[i+1] == '\\')ans += '\\';
                if(str[i+1] == '"')ans += '"';
                i += 1;
                continue;
            }
            else ans += str[i];
        }
        ans += '\0';
    }

    void DivAscii(string str,string &ans){
        int r = 0,l = 0;
        ans = "";
        str = str.substr(1,str.length() - 2);
        for(int i = 0;i < str.length();i++){
            if(str[i] == '\\'){
                if(str[i+1] == 'n')ans += '\n';
                if(str[i+1] == 't')ans += '\t';
                if(str[i+1] == '\\')ans += '\\';
                if(str[i+1] == '"')ans += '"';
                i += 1;
                continue;
            }
            else ans += str[i];
        }
    }

    void DivNWord(string str,int ans[],int &Num){
        int pos = 0;
        Num = 0;
        while(pos < str.length()){
            int tem = 0;
            while(pos < str.length() && (0 > str[pos] - '0' || str[pos] - '0' > 9))
                pos++;

            while(pos < str.length() && 0 <= str[pos] - '0' && str[pos] - '0' <= 9){
                tem = tem * 10 + str[pos] - '0';
                pos++;
            }
            ans[Num++] = tem;
        }
    }
//    void DivThree(string str,int ans[]); //Type指锟斤拷锟斤拷锟酵达拷锟节碉拷0维锟斤拷ans为4维
    void DivOperator(string str,int ans[]){
        int l = 0, r = 0, minor = 0,i = 0;
        string part[3], Type;
        while(r < str.length()){
            while(!IfJump(str[r]) && r < str.length()) r++;
                part[i] = str.substr(l, r - l);
            while(r < str.length() && IfJump(str[r])) r++;
            l = r;
            if(part[i][0] == '$') Type = Type + 'R';
            else if(IsInt(part[i][0])) Type = Type + 'I';
            else Type = Type + 'L';
            i++;
        }
        if(Type == "RRR"){
            ans[0] = 1;
            ans[1] = Register[part[0]];
            ans[2] = Register[part[1]];
            ans[3] = Register[part[2]];
        }
        else if(Type == "RRI") {
            ans[0] = 2;
            ans[1] = Register[part[0]];
            ans[2] = Register[part[1]];
            ans[3] = StringToInt(part[2]);
        }
        else if(Type == "RRL"){
            ans[0] = 3;
            ans[1] = Register[part[0]];
            ans[2] = Register[part[1]];
            ans[3] = Lable[part[2]];
        }
        else if(Type == "RIL"){
            ans[0] = 4;
            ans[1] = Register[part[0]];
            ans[2] = StringToInt(part[1]);
            ans[3] = Lable[part[2]];
        }
        else if(Type == "RR"){
            ans[0] = 5;
            ans[1] = Register[part[0]];
            ans[2] = Register[part[1]];
            ans[3] = -1;
        }
        else if(Type == "RI"){
            ans[0] = 6;
            ans[1] = Register[part[0]];
            ans[2] = StringToInt(part[1]);
            ans[3] = -1;
        }
        else if(Type == "RL"){
            ans[0] = 7;
            ans[1] = Register[part[0]];
            ans[2] = Lable[part[1]];
        }
        else if(Type == "R"){
            ans[0] = 8;
            ans[1] = Register[part[0]];
            ans[2] = ans[3] = -1;
        }
        else if(Type == "L"){
            ans[0] = 9;
            ans[1] = Lable[part[0]];
            ans[2] = ans[3] = -1;
        }
    }

    void DivLoadandStore(string str,int ans[]){
        int l = 0,r = 0,Len = str.length();
        string part1,part2,part3;
        while(r < Len && !IfJump(str[r])) r++;
        part1 = str.substr(l,r - l);
        ans[1] = Register[part1];
        while(r < Len && IfJump(str[r])) r++;
        l = r;
        while(r < Len && !IfJump(str[r]) && str[r] != '(') r++;
    if(str[r] == '('){
        part2 = str.substr(l,r - l);
        r++;
        l = r;
        while(str[r] != ')') r++;
        part3 = str.substr(l,r - l);
        ans[0] = 10;
        ans[2] = StringToInt(part2);
        ans[3] = Register[part3];
    }
    else if(r == str.length() || IfJump(str[r])){
        part2 = str.substr(l,r - l);
        ans[0] = 11;
        ans[2] = LableAddress[part2];
        }
    }
/*
    RRR = 1
    RRI = 2
    RRL = 3
    RIL = 4
    RR = 5
    RI = 6
    RL = 7
    R = 8
    L = 9
    ROO = 10
    RL(A) = 11
*/
};

class Initialize{
public:
    void InitMap(){
    Lable["syscall"] = 60;
    Order[".align"] = 1;
    Order[".ascii"] = 2;
    Order[".asciiz"] = 3;
    Order[".byte"] = 4;
    Order[".half"] = 5;
    Order[".word"] = 6;
    Order[".space"] = 7;
    Order[".data"] = 8;
    Order[".text"] = 9;
    Order["add"] = 10;
    Order["addu"] = 11;
    Order["addiu"] = 12;
    Order["sub"] = 13;
    Order["subu"] = 14;
    Order["mul"] = 15;   //2
    Order["mulu"] = 16;  //2
    Order["div"] = 17;   //2
    Order["divu"] = 18;  //2
    Order["xor"] = 19;
    Order["xoru"] = 20;
    Order["neg"] = 21;
    Order["negu"] = 22;
    Order["rem"] = 23;
    Order["remu"] = 24;
    Order["li"] = 25;
    Order["seq"] = 26;
    Order["sge"] = 27;
    Order["sgt"] = 28;
    Order["sle"] = 29;
    Order["slt"] = 30;
    Order["sne"] = 31;
    Order["b"] = 32;
    Order["beq"] = 33;
    Order["bne"] = 34;
    Order["bge"] = 35;
    Order["ble"] = 36;
    Order["bgt"] = 37;
    Order["blt"] = 38;
    Order["beqz"] = 39;
    Order["bnez"] = 40;
    Order["blez"] = 41;
    Order["bgez"] = 42;
    Order["bgtz"] = 43;
    Order["bltz"] = 44;
    Order["j"] = 45;
    Order["jr"] = 46;
    Order["jal"] = 47;
    Order["jalr"] = 48;
    Order["la"] = 49;
    Order["lb"] = 50;
    Order["lh"] = 51;
    Order["lw"] = 52;
    Order["sb"] = 53;
    Order["sh"] = 54;
    Order["sw"] = 55;
    Order["move"] = 56;
    Order["mfhi"] = 57;
    Order["mflo"] = 58;
    Order["nop"] = 59;
    Order["syscall"] = 60;

    Register["$0"] = 0;
    Register["$1"] = 1;
    Register["$2"] = 2;
    Register["$3"] = 3;
    Register["$4"] = 4;
    Register["$5"] = 5;
    Register["$6"] = 6;
    Register["$7"] = 7;
    Register["$8"] = 8;
    Register["$9"] = 9;
    Register["$10"] = 10;
    Register["$11"] = 11;
    Register["$12"] = 12;
    Register["$13"] = 13;
    Register["$14"] = 14;
    Register["$15"] = 15;
    Register["$16"] = 16;
    Register["$17"] = 17;
    Register["$18"] = 18;
    Register["$19"] = 19;
    Register["$20"] = 20;
    Register["$21"] = 21;
    Register["$22"] = 22;
    Register["$23"] = 23;
    Register["$24"] = 24;
    Register["$25"] = 25;
    Register["$26"] = 26;
    Register["$27"] = 27;
    Register["$28"] = 28;
    Register["$29"] = 29;
    Register["$30"] = 30;
    Register["$31"] = 31;
    Register["$zero"] = 0;
    Register["$at"] = 1;
    Register["$v0"] = 2;
    Register["$v1"] = 3;
    Register["$a0"] = 4;
    Register["$a1"] = 5;
    Register["$a2"] = 6;
    Register["$a3"] = 7;
    Register["$t0"] = 8;
    Register["$t1"] = 9;
    Register["$t2"] = 10;
    Register["$t3"] = 11;
    Register["$t4"] = 12;
    Register["$t5"] = 13;
    Register["$t6"] = 14;
    Register["$t7"] = 15;
    Register["$s0"] = 16;
    Register["$s1"] = 17;
    Register["$s2"] = 18;
    Register["$s3"] = 19;
    Register["$s4"] = 20;
    Register["$s5"] = 21;
    Register["$s6"] = 22;
    Register["$s7"] = 23;
    Register["$t8"] = 24;
    Register["$t9"] = 25;
    Register["$gp"] = 28;
    Register["$sp"] = 29;
    Register["$fp"] = 30;
    Register["$ra"] = 31;
    }

    void InitInput(string str){
        int TemLine = 0;
//      ifstream in("D:\\tmpPro\\untitled4\\array_test1-mahaojun.s");
//      ifstream in("D:\\tmpPro\\untitled4\\array_test1-mahaojun.s");
//      ifstream in("D:\\tmpPro\\untitled4\\basicopt1-5100309127-hetianxing.s");
//      ifstream in("D:\\tmpPro\\untitled4\\builtin-5140519064-youyurong.s");
//      ifstream in("D:\\tmpPro\\untitled4\\array_test2-mahaojun.s");
//      ifstream in("D:\\tmpPro\\untitled4\\bulgarian-5110379024-wuhang.s");
//      ifstream in("D:\\tmpPro\\untitled4\\class_test-mahaojun.s");
//      ifstream in("D:\\tmpPro\\untitled4\\expr-5110309085-jintianxing.s");
//      ifstream in("D:\\tmpPro\\untitled4\\function_test-huyuncong.s");
//      ifstream in("D:\\tmpPro\\untitled4\\gcd-5090379042-jiaxiao.s");
//      ifstream in("D:\\tmpPro\\untitled4\\hanoi-5100379110-daibo.s");
//      ifstream in("D:\\tmpPro\\untitled4\\hashmap-5100309127-hetianxing.s");
//      ifstream in("D:\\tmpPro\\untitled4\\heapsort-5100379110-daibo.s");        //900W
//      ifstream in("D:\\tmpPro\\untitled4\\horse2-5100309153-yanghuan.s");         //820W
//      ifstream in("D:\\tmpPro\\untitled4\\horse3-5100309153-yanghuan.s");
//      ifstream in("D:\\tmpPro\\untitled4\\horse-5100309153-yanghuan.s");
//      ifstream in("D:\\tmpPro\\untitled4\\lvalue2-5110379024-wuhang.s");
//      ifstream in("D:\\tmpPro\\untitled4\\magic-5100309153-yanghuan.s");
//      ifstream in("D:\\tmpPro\\untitled4\\manyarguments-5100379110-daibo.s");
//      ifstream in("D:\\tmpPro\\untitled4\\maxflow-5100379110-daibo.s");
//      ifstream in("D:\\tmpPro\\untitled4\\multiarray-5100309153-yanghuan.s");
//      ifstream in("D:\\tmpPro\\untitled4\\pi-5090379042-jiaxiao.s");
//      ifstream in("D:\\tmpPro\\untitled4\\prime-5100309153-yanghuan.s");
//      ifstream in("D:\\tmpPro\\untitled4\\qsort-5100379110-daibo.s");
//      ifstream in("D:\\tmpPro\\untitled4\\queens-5100379110-daibo.s");
//      ifstream in("D:\\tmpPro\\untitled4\\spill2-5100379110-daibo.s");
//      ifstream in("D:\\tmpPro\\untitled4\\statement_test-huyuncong.s");
//      ifstream in("D:\\tmpPro\\untitled4\\string_test-huyuncong.s");
//      ifstream in("D:\\tmpPro\\untitled4\\superloop-5090379042-jiaxiao.s");
//      ifstream in("D:\\tmpPro\\untitled4\\tak-5090379042-jiaxiao.s");
//      ifstream in("D:\\tmpPro\\untitled4\\twinprime-5090379042-jiaxiao.s");
		ifstream in(str);
        string Tem;
        while(!in.eof()){
            getline(in, Tem);
            if(Tem[0] == '\t')
                Tem = Tem.substr(1, Tem.length() - 1);
            if(Tem[0] == '.' && Tem[1] == 'd')
                DataPos[Dnum++] = TemLine;
            if(Tem[Tem.length() - 1] == ':'){
                string str = Tem.substr(0,Tem.length()-1);
                Lable[str] = TemLine;
                SLable.push_back(str);
            }
            if(Tem == "main:")
                Mpos = TemLine;   //changed 11:28
            Command.push_back(Tem);
            TemLine++;
        }
        TotalLine = TemLine;
        PC = Mpos;
    }

    void InitStaticMemory(){
        DivideCode Divider;
        AllocateMemory MemoryAllocator;
        string CuttedParameter, str;      //str锟斤拷ascii锟斤拷string
        int Word[50], NumOfWord = 0, OneNum = 0;
        for(int i = 0;i < Dnum;i++){
            int line = DataPos[i] + 1;
            while(Command[line] != ".text" && Command[line] != ".data"){
                int type = Divider.DivOrderType(Command[line], CuttedParameter);
                switch(type){
                    case 2: Divider.DivAscii(CuttedParameter, str);break;
                    case 3: Divider.DivAsciiz(CuttedParameter, str);break;
                    case 1:case 7: OneNum = Divider.StringToInt(CuttedParameter);break;
                    case 4:case 5:case 6: Divider.DivNWord(CuttedParameter, Word, NumOfWord);break;
                }
                switch(type){
                    case 1: MemoryAllocator.Align(OneNum);break;
                    case 2:case 3: MemoryAllocator.AllocateString(str, Hpptr); Hpptr += str.length();break;
                    case 4:	MemoryAllocator.AllocateByte(Word,Hpptr,NumOfWord); Hpptr += NumOfWord; break;
                    case 5:	MemoryAllocator.AllocateHalf(Word,Hpptr,NumOfWord); Hpptr += NumOfWord*2; break;
                    case 6:	MemoryAllocator.AllocateInt(Word ,Hpptr,NumOfWord); Hpptr += NumOfWord*4; break;
                    case 7:	MemoryAllocator.AllocateEmpty(OneNum); break;
                    case 0:	string CuttedLable = Command[line];
                            CuttedLable = CuttedLable.substr(0,CuttedLable.length() - 1);
                            LableAddress[CuttedLable] = Hpptr;
                            break;
                }
                line++;
            }
        }
        for(int i = 0;i < 32;i++)
            Reg[i] = 0;
        Reg[Register["$sp"]] = 2000000;
    }

    void TotalInit(string str){
        InitMap();
        InitInput(str);
        InitStaticMemory();
    }
};

class Execution{
public:
    void Add(int ETOM[],int Parameter[]){
        ETOM[0] = Parameter[0];
        ETOM[1] = Parameter[1];
        ETOM[2] = Parameter[2] + Parameter[3];
        ETOM[3] = Parameter[4];
    }
    void Addu(int ETOM[],int Parameter[]);
    void Addiu(int ETOM[],int Parameter[]);
    void Sub(int ETOM[],int Parameter[]){
        ETOM[0] = Parameter[0];
        ETOM[1] = Parameter[1];
        ETOM[2] = Parameter[2] - Parameter[3];
        ETOM[3] = Parameter[4];
    }
    void Subu(int ETOM[],int Parameter[]);
    void Mul(int ETOM[],int Parameter[]){
        ETOM[0] = Parameter[0];
        ETOM[1] = Parameter[1];
        ETOM[3] = Parameter[4];
        if(ETOM[0] == 5 || ETOM[0] == 6){          //HILO
            long long ans = Parameter[1]*Parameter[2];
            ETOM[1] = ans >> 32;
            ETOM[2] = ans;
        }
        else if(ETOM[0] == 1 || ETOM[0] == 2) ETOM[2] = Parameter[2]*Parameter[3];
    }
    void Mulu(int ETOM[],int Parameter[]);
    void Div(int ETOM[],int Parameter[]){
        ETOM[0] = Parameter[0];
        ETOM[3] = Parameter[4];
        if(ETOM[0] == 5 || ETOM[0] == 6){
            ETOM[1] = Parameter[1]/Parameter[2];
            ETOM[2] = Parameter[1]%Parameter[2];
        }
        else if(ETOM[0] == 1 || ETOM[0] == 2){
            ETOM[1] = Parameter[1];
            ETOM[2] = Parameter[2]/Parameter[3];
        }
    }
    void Divu(int ETOM[],int Parameter[]);
    void Xor(int ETOM[],int Parameter[]){
        ETOM[0] = Parameter[0];
        ETOM[1] = Parameter[1];
        ETOM[2] = Parameter[2]^Parameter[3];
        ETOM[3] = Parameter[4];
    }
    void Xoru(int ETOM[],int Parameter[]);
    void Neg(int ETOM[],int Parameter[]){
        ETOM[0] = Parameter[0];
        ETOM[1] = Parameter[1];
        ETOM[2] = -1*Parameter[2];
        ETOM[3] = Parameter[4];
    }
    void Negu(int ETOM[],int Parameter[]);
    void Rem(int ETOM[],int Parameter[]){
        ETOM[0] = Parameter[0];
        ETOM[1] = Parameter[1];
        ETOM[2] = Parameter[2]%Parameter[3];
        ETOM[3] = Parameter[4];
    }
    void Remu(int ETOM[],int Parameter[]);
    void Li(int ETOM[],int Parameter[]){
        ETOM[0] = Parameter[0];
        ETOM[1] = Parameter[1];
        ETOM[2] = Parameter[2];
        ETOM[3] = Parameter[4];
    }
    void Compare(int ETOM[],int Parameter[]){
        int P1 = Parameter[2],P2 = Parameter[3], ans, Opr = Parameter[4];
        switch(Opr){
            case 26:ans = (P1 == P2);break;
            case 27:ans = (P1 >= P2);break;
            case 28:ans = (P1 > P2); break;
            case 29:ans = (P1 <= P2);break;
            case 30:ans = (P1 < P2); break;
            case 31:ans = (P1 != P2);break;
        }
        ETOM[0] = Parameter[0];
        ETOM[1] = Parameter[1];
        ETOM[2] = ans;
        ETOM[3] = Parameter[4];
    }
    void Branch(int ETOM[],int Parameter[]){ //锟矫碉拷bool
        ETOM[0] = Parameter[0];
        ETOM[1] = Parameter[3];
        ETOM[3] = Parameter[4];
        int P1 = Parameter[1], P2 = Parameter[2], Opr = ETOM[3];
        switch(Opr){
            case 33:ETOM[2] = (P1 == P2);break;
            case 34:ETOM[2] = (P1 != P2);break;
            case 35:ETOM[2] = (P1 >= P2);break;
            case 36:ETOM[2] = (P1 <= P2);break;
            case 37:ETOM[2] = (P1 > P2); break;
            case 38:ETOM[2] = (P1 < P2); break;
        }
    }
    void BranchZ(int ETOM[],int Parameter[]){
        ETOM[0] = Parameter[0];
        ETOM[1] = Parameter[2];
        ETOM[3] = Parameter[4];
        int P1 = Parameter[1], Opr = ETOM[3];
        switch(Opr){
            case 39:ETOM[2] = (P1 == 0);break;
            case 40:ETOM[2] = (P1 != 0);break;
            case 41:ETOM[2] = (P1 <= 0);break;
            case 42:ETOM[2] = (P1 >= 0);break;
            case 43:ETOM[2] = (P1 > 0); break;
            case 44:ETOM[2] = (P1 < 0); break;
        }
    }
    void JumpA(int ETOM[],int Parameter[]) {//Meaningless
        ETOM[0] = Parameter[0];
        ETOM[1] = Parameter[1];
        ETOM[3] = Parameter[4];
    }
    void LoadAndStore(int ETOM[],int Parameter[]){//锟斤拷锟斤拷锟斤拷实锟斤拷诖锟斤拷址
        ETOM[0] = Parameter[0];
        ETOM[1] = Parameter[1];
        ETOM[3] = Parameter[4];
        if(ETOM[0] == 10){
            ETOM[2] = Parameter[2] + Parameter[3];
        }
        else ETOM[2] = Parameter[2];
    }
    void MoveFromHL(int ETOM[],int Parameter[]){//锟斤拷取LoHi锟侥寄达拷锟斤拷值
        ETOM[0] = Parameter[0];
        ETOM[1] = Parameter[1];
        ETOM[2] = Parameter[2];
        ETOM[3] = Parameter[4];
    }
    void Nop(int ETOM[],int Parameter[]){
        ETOM[0] = Parameter[0];
        ETOM[1] = Parameter[1];
        ETOM[2] = Parameter[2];
        ETOM[3] = Parameter[4];
    }
    void SystemCall(int ETOM[],int Parameter[]){
        ETOM[3] = Parameter[4];
        int V0 = Parameter[1];
        int A0 = Parameter[2];
        AllocateMemory Allocator;
            if(V0 == 1){                            //Cout Integer
                cout << A0;
            }
            else if(V0 == 4){                       //Cout string
                while(Memory[A0] != '\0')
                    cout<<Memory[A0++];
            }
            else if(V0 == 5){                       //Cin Integer
//               ifstream is("D:\\tmpPro\\Excution\\array_test1-mahaojun.in");
                int tem;
//                cout<<"Please Print in"<<endl;
                cin >> tem;
                Reg[Register["$v0"]] = tem;;
            }
            else if(V0 == 8){                       //Cin string
                int Length = Reg[Register["$a1"]] - 1;
                if(Length < 0)Length = 0;
                string tem;
//                cout<<"Print string:";
                cin>>tem;
  //              getline(is,tem);
                tem = tem.substr(0,Length);
                Allocator.AllocateString(tem, A0);
            }
            else if(V0 == 9){                       //Allocating Heap Memmory
                Reg[Register["$v0"]] = Hpptr;
                Allocator.AllocateEmpty(A0);
            }
            else if(V0 == 10) exit(0);              //Exit Process
            else if(V0 == 17) {
                Flag = 1;
                return;
            }
    }
    void Execute(int ETOM[],int Para[]){
        int Opr = Para[4];
        if(Opr == 0)Nop(ETOM,Para);
        else if(10 <= Opr && Opr <= 12)Add(ETOM, Para);
        else if(49 <= Opr && Opr <= 55)LoadAndStore(ETOM, Para);
        else if(Opr == 25)             Li(ETOM, Para);
        else if(Opr == 13 || Opr == 14)Sub(ETOM, Para);
        else if(Opr == 15 || Opr == 16)Mul(ETOM, Para);
        else if(Opr == 17 || Opr == 18)Div(ETOM, Para);
        else if(Opr == 60)             SystemCall(ETOM, Para);
        else if(33 <= Opr && Opr <= 38)Branch(ETOM, Para);
        else if(39 <= Opr && Opr <= 44)BranchZ(ETOM, Para);
        else if(Opr == 19 || Opr == 20)Xor(ETOM, Para);
        else if(Opr == 21 || Opr == 22)Neg(ETOM, Para);
        else if(Opr == 23 || Opr == 24)Rem(ETOM, Para);
        else if(26 <= Opr && Opr <= 31)Compare(ETOM, Para);
        else if(Opr == 47 || Opr == 48)JumpA(ETOM, Para);
        else if(Opr == 57 || Opr == 58)MoveFromHL(ETOM, Para);
        else Nop(ETOM, Para);
        ETOM[3] = Para[4];
    }
};

//Five Stages Of Pipeline:
void ClearArray(int A[],int B[],int C[]){
        A[0] = A[1] = A[2] = A[3] = A[4] = 0;
        B[0] = B[1] = B[2] = B[3] = 0;
        C[0] = C[1] = C[2] = C[3] = 0;
}

void FetchInstruction(string &str){
    while(Command[PC] == "" && PC < TotalLine) PC++;         //empty line
    string tem = Command[PC];
    while(tem.back() == ':')tem = Command[++PC];             //Lable line
    str = Command[PC];
};
/*
    RRR = 1
    RRI = 2
    RRL = 3
    RIL = 4
    RR = 5
    RI = 6
    RL = 7
    R = 8
    L = 9
    ROO = 10
    RL(A) = 11
*/
void WriteBack(){
//------------------------Store Data into Register
    int Opr = MTOW[3];
    if(Opr == 0){
        PC++;
        return;
    }
    if((10 <= Opr && Opr <= 14)||(19 <= Opr && Opr <= 31)        //Compute,Compare,Li,Load,move
            ||(49 <= Opr && Opr <= 52)||(Opr == 56)){            //RII
                Reg[MTOW[1]] = MTOW[2];
                RegNeeded[MTOW[1]] = false;
            }

    else if(15 <= Opr && Opr <= 18){                             //mul,div
        if(MTOW[0] == 1 || MTOW[0] == 2){                        //II or RII
            Reg[MTOW[1]] = MTOW[2];
            RegNeeded[MTOW[1]] = false;
        }
        else {
            if(15 <= Opr && Opr <= 16){
                LO = MTOW[2];
                HI = MTOW[1];
            }
            else {
                LO = MTOW[1];
                HI = MTOW[2];
            }
            RegNeeded[33] = RegNeeded[32] = false;
        }
    }
    else if(Opr == 57||Opr == 58) {
        Reg[MTOW[1]] = MTOW[2];
        RegNeeded[MTOW[1]] = false;
    }
//-----------------------Modify PC Register
    if(Opr == 32 || Opr == 45 || Opr == 46){                     //b, j, jr    Line
        PC = MTOW[1];
    }
    else if(33 <= Opr && Opr <= 44){                             //branch     bool + Line
        if(MTOW[2] == 1)
            PC = MTOW[1];
        else PC++;
    }
    else if(Opr == 47 || Opr == 48){                             //jar, jarl   Line
        Reg[Register["$ra"]] = PC + 1;
        RegNeeded[Register["$ra"]] = false;
        PC = MTOW[1];
    }
    else PC++;
}

void MemoryAccess(int MTOW[], int ETOM[]){
    AllocateMemory Allocator;
    MTOW[0] = ETOM[0];
    MTOW[1] = ETOM[1];
    MTOW[2] = ETOM[2];
    MTOW[3] = ETOM[3];
    int Opr = ETOM[3];
    if(50 <= Opr && Opr <= 52)Allocator.Load(MTOW, Opr);
    else if(53 <= Opr && Opr <= 55)Allocator.Store(MTOW, Opr);
}

void ExecuteStage(int ETOM[],int Parameter[]){
    Execution Executor;
    Executor.Execute(ETOM, Parameter);
}

void PreDecode(string temOrder,int Parameter[]){
    DivideCode Divider;
    string CuttedParameter;
    int Opr = Divider.DivOrderType(temOrder, CuttedParameter);
    Parameter[4] = Opr;                                            //++
    if((10 <= Opr && Opr <= 48)||(56 <= Opr && Opr <= 58))
        Divider.DivOperator(CuttedParameter, Parameter);
    else if(49 <= Opr && Opr <= 55)
        Divider.DivLoadandStore(CuttedParameter,Parameter);
    else Parameter[0] = -1;
}

void DataPreparation(int Parameter[],bool RegNeeded[]){
    int Opr = Parameter[4];
    if((10 <= Opr && Opr <= 14)||(26 <= Opr && Opr <= 31) //add-subu,seq-sne,xor-xoru,rem-remu
            || Opr == 19 || Opr == 20 || Opr == 23 || Opr == 24){
        Parameter[2] = Reg[Parameter[2]];
        if(Parameter[0] == 1)
            Parameter[3] = Reg[Parameter[3]];
        RegNeeded[Parameter[1]] = true;
    }
    else if(53 <= Opr && Opr <= 55){             //sb - sw
        Parameter[1] = Reg[Parameter[1]];
        if(Parameter[0] == 10)Parameter[3] = Reg[Parameter[3]];
    }
    else if(49 <= Opr && Opr <= 52){             //la-lw
        if(Parameter[0] == 10)Parameter[3] = Reg[Parameter[3]];
        RegNeeded[Parameter[1]] = true;
    }
    else if(15 <= Opr && Opr <= 18){                      //mul - divu
        if(Parameter[0] == 1){
            Parameter[2] = Reg[Parameter[2]];
            Parameter[3] = Reg[Parameter[3]];
            RegNeeded[Parameter[1]] = true;
        }
        else if(Parameter[0] == 2){
            Parameter[2] = Reg[Parameter[2]];
            RegNeeded[Parameter[1]] = true;
        }
        else if(Parameter[0] == 5){
            Parameter[1] = Reg[Parameter[1]];
            Parameter[2] = Reg[Parameter[2]];
            RegNeeded[32] = RegNeeded[33] = true;
        }
    }
    else if(Opr == 21 || Opr == 22 || Opr == 56) { //neg,negu,move
        Parameter[2] = Reg[Parameter[2]];
        RegNeeded[Parameter[1]] = true;
    }
    else if(Opr == 25) RegNeeded[Parameter[1]] = true;    //li
    else if(33 <= Opr && Opr <= 38){                      //beq - blt
        if(Parameter[0] == 3){
            Parameter[1] = Reg[Parameter[1]];
            Parameter[2] = Reg[Parameter[2]];
        }
        else if(Parameter[0] == 4)
            Parameter[1] = Reg[Parameter[1]];
    }
    else if((39 <= Opr && Opr <= 44) || Opr == 46)               //beqz-bltz, jr
        Parameter[1] = Reg[Parameter[1]];
    else if(Opr == 47){                             //jar ,jarl
        RegNeeded[Register["$ra"]] = true;
    }
    else if(Opr == 48){
        Parameter[1] = Reg[Parameter[1]];
        RegNeeded[Register["$ra"]] = true;
    }
    else if(Opr == 57){                          //mfhi
        Parameter[2] = HI;
        RegNeeded[Parameter[1]] = true;
    }
    else if(Opr == 58){                          //mflo
        Parameter[2] = LO;
        RegNeeded[Parameter[1]] = true;
    }
    else if(Opr == 60){                          //systemcall
        Parameter[0] = 12;
        Parameter[1] = Reg[Register["$v0"]];
        Parameter[2] = Reg[Register["$a0"]];
        RegNeeded[Register["$v0"]] = RegNeeded[Register["$a0"]] = RegNeeded[Register["$a1"]] = true;
    }
}

bool JudgeDataHazard(int Parameter[], bool RegNeeded[]){
    int P0 = Parameter[0], P1 = Parameter[1], P2 = Parameter[2], P3 = Parameter[3], P4 = Parameter[4];
    if(P0 == 1) {if(RegNeeded[P2] || RegNeeded[P3]) return 1;}
    else if(P0 == 2){ if(RegNeeded[P2]) return 1;}
    else if(P0 == 3){ if(RegNeeded[P1] || RegNeeded[P2]) return 1;}
    else if(P0 == 4){ if(RegNeeded[P1]) return 1;}
    else if(P0 == 5){
        if(15 <= P4 && P4 <= 18){
            if(RegNeeded[32]||RegNeeded[33])
                return 1;
        }
        if(RegNeeded[P1] || RegNeeded[P2])
            return 1;
    }
    else if(P0 == 6){
        if(15 <= P4 && P4 <= 18){
            if(RegNeeded[32]||RegNeeded[33]||RegNeeded[P1])
                return 1;
        }
        else if((P4 != 25) && (RegNeeded[P1]))
            return 1;
    }
    else if(P0 == 7){ if(RegNeeded[P1]) return 1;}
    else if(P0 == 8){
        if((P4 == 57 || P4 == 58)){
            if(RegNeeded[32] || RegNeeded[33]||RegNeeded[P1])
                return 1;
        }
        else if(RegNeeded[P1])
            return 1;
    }
    else if(P0 == 10){               //ROO
        if(RegNeeded[P3])
            return 1;
        if(53 <= P4 && P4 <= 55 && RegNeeded[P1]) //Save
            return 1;
    }
    else if(P0 == 11){               //RL Save
        if(53 <= P4 && P4 <= 55 && RegNeeded[P1])
            return 1;
    }

    if(P4 == 60){
        if(RegNeeded[Register["$v0"]] || RegNeeded[Register["$a0"]] || RegNeeded[Register["$a1"]])
            return 1;
    }
    return false;
}

void SolvingControlHazard(int Parameter[], int ETOM[], int MTOW[],bool RegNeeded[]){
    int TemPC = PC - 1;
    WriteBack();
    MemoryAccess(MTOW, ETOM);
    ExecuteStage(ETOM, Parameter);
    WriteBack();
    MemoryAccess(MTOW, ETOM);
    PC = TemPC;                                     //回到从前第一条WriteBack做决策时的PC
    WriteBack();
    ClearArray(Parameter, ETOM, MTOW);
}

void SolvingDataHarzard(int ETOM[] ,int MTOW[],bool RegNeeded[]){
    int TemPC = PC - 1;
    WriteBack();
    MemoryAccess(MTOW, ETOM);
    PC = TemPC;
    WriteBack();
    ETOM[0] = ETOM[1] = ETOM[2] = ETOM[3] = 0;
    MTOW[0] = MTOW[1] = MTOW[2] = MTOW[3] = 0;
}

void DecodeAndDataPreparation(string temOrder, int Parameter[],int ETOM[],int MTOW[],bool RegNeeded[]){
    PreDecode(temOrder, Parameter);
    int Opr = Parameter[4];
    int Hazard = JudgeDataHazard(Parameter, RegNeeded);
    if(Hazard) SolvingDataHarzard(ETOM, MTOW, RegNeeded);
    if(32 <= Opr && Opr <= 48) {
        DataPreparation(Parameter,RegNeeded);
        SolvingControlHazard(Parameter, ETOM, MTOW, RegNeeded);
    }
    else DataPreparation(Parameter,RegNeeded);
}


int main(int argc,char *argv[]) {
    Initialize Initializer;
    Initializer.TotalInit(argv[1]);
    string Instruction;
    while(true){
        if(Flag) return Reg[Register["$a0"]];
        WriteBack();
        MemoryAccess(MTOW, ETOM);
        ExecuteStage(ETOM, Parameter);
        DecodeAndDataPreparation(Instruction, Parameter, ETOM, MTOW, RegNeeded);
        FetchInstruction(Instruction);
    }
}











