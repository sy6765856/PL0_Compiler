#include <stdio.h>
#include <string.h>
const int N=10000;
const int INF=0x3f3f3f3f;
const int LEN=14;
const int WORD_LEN=10000;
char line[N];
int cnt,error_cnt;
char dict[300][20];
char op[300][10];
char ch[LEN]={'=',',','>','<',';',':','+','-','*','/','(',')','#','#'};
char word[WORD_LEN];
enum symbol
    {
        nul,ident,number,plus,minus,times,slash,oddsym,eql,neq,lss,leq,gtr,geq,lparen,rparen,comma,semicolon,period,
        becomes,beginsym,endsym,ifsym,thensym,whilesym,writesym,readsym,dosym,callsym,constsym,varsym,procsym
    };
int check(char key)
{
    int i;
    for(i=0;i<LEN;i++)
        if(key==ch[i])return i;
    return -1;
}
int check_two(char key[])
{
    if(key[0]==':'&&key[1]=='=')return 1;
    if(key[0]=='='&&key[1]=='=')return 1;
    if(key[0]=='>'&&key[1]=='=')return 1;
    if(key[0]=='<'&&key[1]=='=')return 1;
    return 0;
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
void type(char wd[])
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
    if(id==-1){id=32;error_cnt++;}
    printf("%-15s",dict[id]);
    if(id==2)printf("%-15lf",val);
}
void print(char wd[])
{
    printf("%-15d%-15s",++cnt,wd);type(wd);puts("");
}
void lexi(char line[])
{
    int len=strlen(line),st=0,i,cp=0;
    while(st<len)
    {
        int fg=check(line[st]);
        if(line[st]==' '||line[st]=='\t'||line[st]=='\n')fg=INF;
        if(~fg)
        {
            if(cp){word[cp]='\0';print(word);cp=0;}
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
                        print(wd);
                        st++;fgg=0;
                    }
                }
                if(fgg){wd[1]='\0';print(wd);}
            }  
        }
        else if(line[st]=='.')
        {
            if(cp&&word[cp-1]>='0'&&word[cp-1]<='9'&&st<len-1&&line[st+1]>='0'&&line[st+1]<='9')
                word[cp++]=line[st];
            else
            {
                if(cp){word[cp]='\0';print(word);cp=0;}
                word[cp++]=line[st];
            }
        }
        else word[cp++]=line[st];
        st++;
    }
    if(cp){word[cp]='\0';print(word);cp=0;}
}
void solve(FILE *fp)
{
    cnt=0;error_cnt=0;
    while(gets(line)!=NULL){printf("%-15d",++cnt);puts(line);}
    puts("");
    cnt=0;error_cnt=0;rewind(fp);
    printf("%-15s%-15s%-15s%-15s\n","ID","NAME","TYPE","VALUE");
    while(gets(line)!=NULL)
    lexi(line);
    printf("%d errors.\n",error_cnt);
}
int main(int argc, char *argv[])
{
    init();
    FILE *fp=freopen("in","r",stdin);
    solve(fp);
    freopen("out","w",stdout);
    rewind(fp);solve(fp);
    return 0;
}
