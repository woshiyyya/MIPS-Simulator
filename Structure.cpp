//MIPS���
map<stirng ,int> Order,Lable;
map<int,int> Line;
Order[order] = int;   //ָ���ӳ��
Lable[lable] = int;   //Lable��Ӧ���к�
int Register[16];
char Memory[4M];
int PC,TemLine;   //PC�Ĵ������Ѷ�ָ��,��ǰ����
vector<string> Command;  //ָ��ж���


The First Traverse��
  int DataPos[] = line;//��¼.data���ֵ�����,
  int Dnum,Mpos; // ��ǰ.data����;main:λ��
  freopen("File","r",stdin);

  while(!cin.eof()){
    getline(cin,Tem);
    if(Tem[0] == '\t')Tem = Tem.substr(1,Tem.length() - 1);
    Command.push_back(Tem);
    TemLine++;
    //ʶ���������
    switch(Tem){
      case  Lable:  //judge by ':'
        Lable[lable] = Temline;
        Line[TemLine] = type;
      case  .data:
        DataPos[Dnum++] = Temline;
        Line[TemLine] = type;
      case  main:
        Mpos = Temline;
        Line[TemLine] = type;
    }

  }

//Decode �� FetchData
  class DivideParameter{
    int DivOrderType(string str,string &ans);//ʶ��Order���������ͣ����и���֮��Ĳ���
    //ʹ�� ans = str.substr(off,count) �и�str[off]~str[off + count - 1];
    void DivAsciiz(string str,string &ans);//ת���ַ�
    void DivAscii(string str,string &ans);
    void DivWord(string str,int ans[],int &Num);
    void DivByte(string str,char ans[],int &Num);
    void DivHalf(string str,char ans[],int &Num);
    void DivOperator(string str,int ans[]); //Typeָ�����ʹ��ڵ�0ά��ansΪ4ά
    void DivLoadandStore(string str,int ans[]);
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
    systemcall = 12 ---> $v0 $a0
    NULL = -1��
    */
  }

  class Execution{
      //�������
      int add(int data[]);
      int addu(int data[]);
      ....
      //�Ƚ�ָ��
      bool seq(int data[]);
      bool sne(int data[]);
      ....
      //��ת�Ƚ�
      bool beq(int data[]);
      bool bnez(int data[]);
      .........................................
      //����Ϊ��ת���Ժ���
      void jump(int data[],bool flag){
        if(flag)PC = address;
        if(jarl || jar)$ra = PC + 1;
      };
      void branch(int data[],bool flag){
        if(flag)PC = address;
      };
  }
//MemoryAccess �� WriteBack
  class AllocateMemory{
    private:
    int hptr;//��ָ��
    class BinaryTransForm{
        void IntToBinary(,char ans[],int data[],int NumOfInt,int Type = 1);//change
        int BinaryToInt(char data[],int ans[],int NumOfInt);
    }//int ת����Ƕ��

    public:
    void AllocateInt(int data[],int pos,int NumOfInt){
          compute TOTBIT;
          char* ANS = new char[TOTBIT];
          BinaryTransForm(data,int Num,ANS);
          Write On Memory;
    }
    void AllocateString(string data,int pos){     //��д
          Write On Memory;
    }
    void AllocateEmpty(int Num,int pos){
          change the heapptr;
    }//align,space,byte,half,systemcall9
    void Load(int data[],int ans[],int OperationType);
    Execution����Memory Access����ans[2]ΪĿ��Ĵ��� + �ڴ��ַ��
    Memory Access����Write Back�����ans[2]ΪĿ��Ĵ��� + ֵ��
    void Store(int data[],int ans[],int OperationType){
      //ʵ�ʵ��÷����AllocateString��AllocateInt��
    };
  }

  The Second TRAVERSE
    for(int i 0:Dnum)
      dataPos//���뾲̬�ռ䣻
    PC = Mpos;
    int Parameter[4];
    whlie(now < Order.size){
        string NowLine = Order[PC];
        int TypeOfLine = DivOrderType(NowLine,Cutted);
        switch(TypeOfLine){
          case 1:
            DivThree(Cutted,Parameter)
            int ans = Execute.add(Parameter);
            Register[Parameter[1]] = ans;
            PC = PC + 1;
          ........
          case x:
            int ans = Execute.funcx();
          ........
          case systemcall:{
            switch($v0)
              case 1,4,5,8,9,10,17
          }
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
    systemcall = 12 ---> $v0 $a0
    NULL = -1��
    */
    
    class Execution //����Parameter[4]��OperationType��
                    //����EToM[3] ԭʼParameter���� + �Ĵ������/Address + ֵ
{
    void add(int EToM[],int Parameter[]){
        EToM[0] = Parameter[0];
        EToM[1] = Parameter[1];
        EToM[2] = Parameter[1] + Parameter[2];                
    }   
    ��� �Ĵ��� + ֵ 
    void Add(int EToM[],int Parameter[])
    void Addu(int EToM[],int Parameter[])
    void Addiu(int EToM[],int Parameter[])
    void Sub(int EToM[],int Parameter[])
    void Subu(int EToM[],int Parameter[])
    void Mul(int EToM[],int Parameter[])
    void Mulu(int EToM[],int Parameter[])
    void Div(int EToM[],int Parameter[])
    void Divu(int EToM[],int Parameter[])
    void Xor(int EToM[],int Parameter[])
    void Xoru(int EToM[],int Parameter[])
    void Neg(int EToM[],int Parameter[])
    void Negu(int EToM[],int Parameter[])
    void Rem(int EToM[],int Parameter[])
    void Remu(int EToM[],int Parameter[])
    void Li(int EToM[],int Parameter[])
    void Compare(int EToM[],int Parameter[])  //�����߱Ƚϵ�bool
    void Branch(int EToM[],int Parameter[])     //�õ�bool
    void BranchZ(int EToM[],int Parameter[]) //�õ��жϵ�boolֵ
    void JumpA(int EToM[],int Parameter[])  //׼����һ��ָ����к�
    void LoadAndStore(int EToM[],int Parameter[])//������ʵ��ڴ��ַ
    Load ���R + Address��Store ���ֵ + Address��
    void MoveFromHL(int EToM[],int Parameter[])//��ȡLoHi�ļĴ���ֵ
    void SystemCall(int EToM[],int Parameter[])
              
}
  
      

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

