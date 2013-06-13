#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#define color \e[1;31m
#define clear \e[0m
#define _cons(a,b) printf("  %10s%40s\n",a,b)
#define cons(a,b) _cons(a,b)
using namespace std;
const int N=100000;
const int INF=0x3f3f3f3f;
const int LEN=13;
const int WORD_LEN=10000;
char line[N];
int cnt,error_cnt,n,line_cnt;
char dict[300][20],op[300][10];
char ch[LEN]={'=',',','>','<',';',':','+','-','*','/','(',')','#'};
char word[WORD_LEN];
enum symbol
    {
        nul,ident,number,plussym,minusym,times,slash,oddsym,eql,neq,lss,leq,gtr,geq,lparen,rparen,comma,semicolon,period,
        becomes,beginsym,endsym,ifsym,thensym,whilesym,writesym,readsym,dosym,callsym,constsym,varsym,procsym
    };
enum error_type
{
    error_info,semicolon_missing,begin_missing,end_missing,period_missing,constant_error,
    ident_missing,procedure_error,error,lparen_missing,rparen_missing,becomes_missing,then_missing,do_missing,wrong_ident
};
string error_message[16]=
    {"error info","expect   ';'","expect   'begin'  before this line","expect   'end'","expect   '.'","constant error!",
     "expect   ident","procedure error","error","expect   '('","expect   ')'","expect   ':='",
     "expect   'then'","expect   'do'","wrong ident"};
struct list
{
    int line_num;
    int error_info;
    int lev;
}error_list[N];
int words[N],line_num[N];
///----------------------------lexi-----------------------------///
int check(char key)
{
    int i;
    for(i=0;i<LEN;i++)
        if(key==ch[i])return i;
    return -1;
}
bool check_two(char key[])
{
    if(key[0]==':'&&key[1]=='=')return true;
    if(key[0]=='='&&key[1]=='=')return true;
    if(key[0]=='>'&&key[1]=='=')return true;
    if(key[0]=='<'&&key[1]=='=')return true;
    return false;
}
void init()
{
    strcpy(dict[0],"nul");strcpy(op[0]," ");
    strcpy(dict[1],"ident");strcpy(op[1],"indent");
    strcpy(dict[2],"number");strcpy(op[2],"number");
    strcpy(dict[3],"plus");strcpy(op[3],"+");
    strcpy(dict[4],"minus");strcpy(op[4],"-");
    strcpy(dict[5],"times");strcpy(op[5],"*");
    strcpy(dict[6],"slash");strcpy(op[6],"/");
    strcpy(dict[7],"oddsym");strcpy(op[7],"odd");
    strcpy(dict[8],"eql");strcpy(op[8],"==");
    strcpy(dict[9],"neq");strcpy(op[9],"#");
    strcpy(dict[10],"lss");strcpy(op[10],"<");
    strcpy(dict[11],"leq");strcpy(op[11],"<=");
    strcpy(dict[12],"gtr");strcpy(op[12],">");
    strcpy(dict[13],"geq");strcpy(op[13],">=");
    strcpy(dict[14],"lparen");strcpy(op[14],"(");
    strcpy(dict[15],"rparen");strcpy(op[15],")");
    strcpy(dict[16],"comma");strcpy(op[16],",");
    strcpy(dict[17],"semicolon");strcpy(op[17],";");
    strcpy(dict[18],"period");strcpy(op[18],".");
    strcpy(dict[19],"becomes");strcpy(op[19],":=");
    strcpy(dict[20],"beginsym");strcpy(op[20],"begin");
    strcpy(dict[21],"endsym");strcpy(op[21],"end");
    strcpy(dict[22],"ifsym");strcpy(op[22],"if");
    strcpy(dict[23],"thensym");strcpy(op[23],"then");
    strcpy(dict[24],"whilesym");strcpy(op[24],"while");
    strcpy(dict[25],"writesym");strcpy(op[25],"write");
    strcpy(dict[26],"readsym");strcpy(op[26],"read");
    strcpy(dict[27],"dosym");strcpy(op[27],"do");
    strcpy(dict[28],"callsym");strcpy(op[28],"call");
    strcpy(dict[29],"constsym");strcpy(op[29],"const");
    strcpy(dict[30],"varsym");strcpy(op[30],"var");
    strcpy(dict[31],"procsym");strcpy(op[31],"procedure");
    strcpy(dict[32],"error");
}
void type(char wd[],int line_cnt)
{
    int len=strlen(wd),id=-1;
    double val;
    for(int i=0;i<32;++i)if(strcmp(wd,op[i])==0){id=i;break;}
    if(id==-1)
    {
        int fg=1;
        if('a'<=wd[0]&&wd[0]<='z'||'A'<=wd[0]&&wd[0]<='Z')
        {
            for(int i=1;i<len;++i)
            {
                if('a'<=wd[i]&&wd[i]<='z'||'A'<=wd[i]&&wd[i]<='Z'||wd[i]=='_'||'0'<=wd[i]&&wd[i]<='9')continue;
                fg=0;
            }
            if(fg)id=1;
        }
        else if('0'<=wd[0]&&wd[0]<='9')
        {
            val=wd[0]-'0';
            int yu=1;
            for(int i=1;i<len;++i)
            {
                if(wd[i]=='.'){yu=i;break;}
                if(!('0'<=wd[i]&&wd[i]<='9')){yu=0;break;}
                val=val*10+wd[i]-'0';
            }
            if(yu)
            {
                double dv=1;
                for(int i=yu+1;i<len;++i)
                {
                    if(!('0'<=wd[i]&&wd[i]<='9')){yu=0;break;}
                    dv/=10;
                    val+=(wd[i]-'0')*dv;
                }
            }
            if(yu)id=2;
        }
    }
    if(id==-1)
    {
        id=32;
        error_list[error_cnt].line_num=line_cnt;
        error_list[error_cnt++].error_info=wrong_ident;
    }
    printf("%-18s",dict[id]);
    words[cnt-1]=id;
    if(id==2)printf("%-18lf",val);
}
void print(char wd[],int line_cnt)
{
    printf("%-18d%-18s",++cnt,wd);
    line_num[cnt-1]=line_cnt;
    type(wd,line_cnt);puts("");
}
void lexi(char line[],int line_cnt)
{
    int len=strlen(line),st=0,i,cp=0;
    while(st<len)
    {
        int fg=check(line[st]);
        if(line[st]==' '||line[st]=='\t'||line[st]=='\n')fg=INF;
        if(~fg)
        {
            if(cp){word[cp]='\0';print(word,line_cnt);cp=0;}
            if(fg!=INF)
            {
                int fgg=1;
                char wd[4];
                wd[0]=line[st];
                if(st<len-1)
                {
                    wd[1]=line[st+1];
                    wd[2]='\0';
                    if(check_two(wd))
                    {
                        print(wd,line_cnt);
                        st++;fgg=0;
                    }
                }
                if(fgg){wd[1]='\0';print(wd,line_cnt);}
            }
        }
        else if(line[st]=='.')
        {
            if(cp&&word[cp-1]>='0'&&word[cp-1]<='9'&&st<len-1&&line[st+1]>='0'&&line[st+1]<='9')
                word[cp++]=line[st];
            else
            {
                if(cp){word[cp]='\0';print(word,line_cnt);cp=0;}
                word[cp++]=line[st];
            }
        }
        else word[cp++]=line[st];
        st++;
    }
    if(cp){word[cp]='\0';print(word,line_cnt);cp=0;}
}
void solve(FILE *fp)
{
    line_cnt=0;error_cnt=0;
    puts("\e[1;33m ");
    for(int i=0;i<60;i++)printf("#");puts("");
    puts("\e[1;34m ");
    while(gets(line)!=NULL){printf("%-18d",++line_cnt);puts(line);}
    puts("\e[1;33m ");
    for(int i=0;i<60;i++)printf("#");puts("\n\n");
    puts("\e[0m");
    line_cnt=0;cnt=0;error_cnt=0;rewind(fp);
    printf("%-18s%-18s%-18s%-18s\n","ID","NAME","TYPE","VALUE");
    for(int i=0;i<60;i++)printf("-");puts("");
    while(gets(line)!=NULL)
    lexi(line,++line_cnt);
    for(int i=0;i<60;i++)printf("-");puts("");
}
///----------------------------lexi-----------------------------///


///*---------------------------Parse----------------------------///
#define add_err(s) add_error(line_num[id],s,lev)
#define add_err_retf(s) add_err(s),ret=false
#define Parameter int words[],int line_num[],int &id,int n,int lev
#define para words,line_num,id,n
#define put() for(int i=0;i<=lev;i++)printf("  ");
bool sub_program(Parameter);
bool procedure(Parameter);
bool expression(Parameter);
void add_error(int row,int error,int lev) // Add an error to the error_list
{
    error_list[error_cnt++]={row,error,lev};
}
bool constant(Parameter)        // check constant
{
    put();printf("[%d]const\n",lev);
    int lv=id;
    bool ret=true;
    do
    {
        if(words[id]==ident&&words[id+1]==eql&&words[id+2]==number)
            id+=3;
        else add_err_retf(constant_error);
    }while(words[id]==comma);
    if(words[id]!=semicolon)
        add_err_retf(semicolon_missing);
    //for(int i=lv;i<=id;i++)printf("%s ",dict[words[i]]);puts("");
    id++;
    return ret;
}
bool variable(Parameter)        // check variable
{
    put();printf("[%d]var\n",lev);
    int lv=id;
    bool ret=true;
    do
    {
        if(words[id]==ident)id++;
        else add_err_retf(ident_missing);
    }while(words[id++]==comma);
    id--;
    if(words[id]!=semicolon)
        add_err_retf(semicolon_missing);
    //for(int i=lv;i<=id;i++)printf("%s ",dict[words[i]]);puts("");
    id++;
    return ret;
}
bool procedure(Parameter)       // check procedure
{
    put();printf("[%d]proc\n",lev);
    int lv=id;
    bool ret=true;
    while(words[id]==procsym)
    {
        id+=3;
        if(!(words[id-2]==ident&&words[id-1]==semicolon))
            add_err_retf(procedure_error);
        if(!sub_program(para,lev+1))ret=false;
        if(words[id]!=semicolon)
            add_err_retf(semicolon_missing);
        //for(int i=lv;i<=id;i++)printf("%s ",dict[words[i]]);puts("");
        id++;
    }return ret;
}
#define express(lev) expression(para,lev)
bool condition(Parameter)       // check condition
{
    put();printf("[%d]condition\n",lev);
    bool ret=true;
    if(words[id]==oddsym)
    {
        id++;
        if(!express(lev+1))ret=false;
    }
    else
    {
        if(!express(lev+1))ret=false;
        if(!(words[id]==eql||words[id]==neq||words[id]==lss||words[id]==leq
             ||words[id]==gtr||words[id]==geq))ret=false;
        id++;
        if(!express(lev+1))ret=false;
    }return ret;
}
bool factor(Parameter)          // check factor
{
    put();printf("[%d]factor\n",lev);
    int lv=id;
    bool ret=true;
    if(words[id]==ident||words[id]==number)
    {
        id++;
        return ret;
    }
    if(words[id]!=lparen)
        add_err_retf(lparen_missing);
    id++;
    if(!express(lev+1))ret=false;
    if(words[id]!=rparen)
        add_err_retf(rparen_missing);
    // for(int i=lv;i<=id;i++)printf("%s ",dict[words[i]]);puts("");
    return ret;
}
bool item(Parameter)            // check item
{
    put();printf("[%d]item\n",lev);
    bool ret=true;
    if(!factor(para,lev+1))ret=false;
    while(words[id]==times||words[id]==slash)
    {
        id++;
        if(!factor(para,lev+1))ret=false;
    }return ret;
}
bool expression(Parameter)      // check expression
{
    put();printf("[%d]expression\n",lev);
    bool ret=true;
    if(words[id]==plussym||words[id]==minusym)id++;
    if(!item(para,lev+1))ret=false;
    while(words[id]==plussym||words[id]==minusym)
    {
        id++;
        if(!item(para,lev+1))ret=false;
    }return ret;
}
#define state(lev) statement(para,lev)
bool statement(Parameter)       // check statement
{
    put();printf("[%d]statement\n",lev);
    bool ret=true;
    if(words[id]==ident)
    {
        id++;
        if(words[id]!=becomes)
            add_err_retf(becomes_missing);
        id++;
        if(!express(lev+1))ret=false;
    }
    if(words[id]==callsym)
    {
        if(words[id+1]!=ident)
            add_err_retf(ident_missing);
        id+=2;
    }
    if(words[id]==beginsym)
    {
        id++;
        if(!state(lev+1))ret=false;
        while(words[id]==semicolon)
        {
            id++;
            if(!state(lev+1))ret=false;
        }
        if(words[id]!=endsym)
            add_err_retf(end_missing);
        id++;
    }
    if(words[id]==ifsym)
    {
        put();printf("[%d]if\n",lev);
        id++;
        if(!condition(para,lev+1))ret=false;
        if(words[id]!=thensym)
            add_err_retf(then_missing);
        id++;
        if(!state(lev+1))ret=false;
    }
    if(words[id]==whilesym)
    {
        put();printf("[%d]while\n",lev);
        id++;
        if(!condition(para,lev+1))ret=false;
        if(words[id]!=dosym)
            add_err_retf(do_missing);
        id++;
        if(!state(lev+1))ret=false;
    }
    if(words[id]==readsym)
    {
        put();printf("[%d]read\n",lev);
        if(words[++id]!=lparen)
            add_err_retf(lparen_missing);
        if(words[++id]!=ident)ret=false;
        id++;
        while(words[id]==comma)
        {
            if(words[++id]!=ident)ret=false;
            id++;
        }
        if(words[id]!=rparen)
            add_err_retf(rparen_missing);
        id++;
    }
    if(words[id]==writesym)
    {
        put();printf("[%d]write\n",lev);
        id++;
        if(words[id]!=lparen)
            add_err_retf(lparen_missing);
        id++;
        if(!express(lev+1))ret=false;
        while(words[id]==comma)
        {
            id++;
            if(!express(lev+1))ret=false;
        }
        if(words[id]!=rparen)
            add_err_retf(rparen_missing);
        id++;
    }return ret;
}
bool sub_program(Parameter)     // check sub_program
{
    put();printf("[%d]sub_program\n",lev);
    bool ret=true;
    if(words[id]==constsym)
    {
        id++;
        if(!constant(para,lev))ret=false;
    }
    if(words[id]==varsym)
    {
        id++;
        if(!variable(para,lev))ret=false;
    }
    if(!procedure(para,lev))ret=false;
    if(!state(lev))ret=false;
    return ret;
}
bool program(int words[],int line_num[],int n,int lev) // check program
{
    bool ret=true;
    int id=0;n--;
    if(!sub_program(para,lev))ret=false;
    if(words[n]!=period)
    {
        add_error(line_num[n],period_missing,lev);
        ret=false;
    }return ret;
}
bool check_begin(int words[],int line_num[],int n,int lev)
{
    bool ret=true;
    int stack_begin=0;
    for(int id=0;id<n;id++)
    {
        if(words[id]==beginsym)stack_begin++;
        else if(words[id]==endsym)
        {
            if(stack_begin)stack_begin--;
            else
                add_err_retf(begin_missing);
        }
    }return ret;
}
void print_error()              // print errors in error_list
{
    for(int i=0;i<error_cnt;i++)
    {
        for(int j=0;j<=error_list[i].lev;j++)printf("  ");
        printf("\e[1;33m%s\e[0m: \e[1;41m[%d]\e[0m\e[1;31m error\e[0m: line \e[1;32m%d\e[0m :   ",__FILE__,error_list[i].lev,error_list[i].line_num);
        cout<<"\e[1;40m"<<error_message[error_list[i].error_info]<<"\e[0m"<<endl;
    }
}
///----------------------------Parse----------------------------///
int main(int argc, char *argv[])
{
    init();
    FILE *fp=freopen("in","r",stdin);
    solve(fp);
//    freopen("out.txt","w",stdout);
//    rewind(fp);solve(fp);
    ///parse
    puts("\e[1;35m ");
    cons(__DATE__,__TIME__);
    puts("\e[0m");
    if(program(words,line_num,cnt,0)&&check_begin(words,line_num,cnt,0))
        puts("Complie finished!");
    print_error();
    printf("\n  \e[1;41m%d\e[0m\e[1;31m errors\e[0m.\n",error_cnt);
    return 0;
}
