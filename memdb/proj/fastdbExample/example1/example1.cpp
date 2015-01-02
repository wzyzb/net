// example1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "stdio.h"
#include "string.h"
#include "fastdb.h"

USE_FASTDB_NAMESPACE ;

/*
RELATION(reference,inverse_reference)

指定类（表）之间的一对一、一对多或者多对多的关系。reference和inverse_reference字段都必须是引用或者引用数组类型。
inverse_reference字段是一个包含了指向当前表的逆引用的引用表。逆引用自动由fastdb更新并用于查询优化。
*/

class Score;
class Student
{
public:
    char const* vc_stuid;
    char const* vc_stuname;
    char const* vc_sex;
    dbArray< dbReference<Score> >  ScoreRefs;
    TYPE_DESCRIPTOR((KEY(vc_stuid,INDEXED|HASHED),
                    KEY(vc_stuname,HASHED),
                    FIELD(vc_sex),
                    RELATION(ScoreRefs,StuRef)
        ));
};

class Subject
{
public:
    char const* vc_subid;
    char const* vc_subname;
    dbArray<dbReference<Score> >    ScoreRefs;

    TYPE_DESCRIPTOR((
                    KEY(vc_subid,INDEXED|HASHED),
                    KEY(vc_subname,HASHED),
                    RELATION(ScoreRefs,SubRef)
        ));
};

class Score
{
public:
    char const* vc_stuid;
    char const* vc_subid;
    int4        l_score;
    dbReference<Student>    StuRef;
    dbReference<Subject>    SubRef;
    TYPE_DESCRIPTOR((
                    KEY(vc_stuid,INDEXED|HASHED),
                    KEY(vc_subid,INDEXED|HASHED),
                    FIELD(l_score),
                    RELATION(StuRef,ScoreRefs),
                    RELATION(SubRef,ScoreRefs)
        ));
};

REGISTER(Student);
REGISTER(Subject);
REGISTER(Score);

int _tmain(int argc, _TCHAR* argv[])
{
    const int maxStrlen = 256;
    dbDatabase  db;
    Student stu;
    Subject sub;
    Score   sco;
    char name[101];
    char name1[101];
    char id[11];
    char id1[4];
    strcpy(name,"wangbo");
    strcpy(name1,"tanghui");

    dbQuery q1,q2,q3,q4,q5;
    q1 = "vc_stuname =",name;
    q2 = "vc_stuname =",name1;
    q3 = "vc_stuid =",id;
    q4 = "vc_subid =",id;

    dbCursor<Student>   students;
    dbCursor<Score>     scores;
    dbCursor<Subject>   subjects;
    dbCursor<Student>   updatestudent(dbCursorForUpdate);

    if (db.open("FastDbtest"))
    {
        char stuid[11];
        char stuname[101];
        char sex[2];
        char subid[4];
        char subname[101];
        int  score;
        memset((void*)stuid,'\0',sizeof(stuid));
        memset((void*)stuname,'\0',sizeof(stuname));
        memset((void*)sex,'\0',sizeof(sex));
        memset((void*)subid,'\0',sizeof(subid));
        memset((void*)subname,'\0',sizeof(subname));

        strcpy(stuid,"1");
        strcpy(stuname,"wangjm");
        strcpy(sex,"0");
        strcpy(id,stuid);
        if (students.select(q3) >0 )
        {
            printf("this student is existed!\n");
        }
        else
        {
            stu.vc_stuid = stuid;
            stu.vc_stuname = stuname;
            stu.vc_sex = sex;
            insert(stu);
            db.commit();
        }

        strcpy(stuid,"2");  
        strcpy(stuname,"tanghui");  
        strcpy(sex,"1");  
        strcpy(id,stuid);  
        if (students.select(q3) >0 )  
        {  
            printf("this student is existed!\n");  
        }  
        else  
        {  
            stu.vc_stuid = stuid;  
            stu.vc_stuname = stuname;  
            stu.vc_sex = sex;  
            insert(stu);  
            db.commit();  
            printf("student infomation insert successfully!\n");  
        }  
        
        strcpy(subid,"1");  
        strcpy(subname,"c++");  
        strcpy(id1,subid);  
        if (subjects.select(q4) >0)  
        {  
            printf("this subject is existed!\n");  
        }  
        else  
        {  
            sub.vc_subid = subid;  
            sub.vc_subname = subname;  
            insert(sub);  
            db.commit();  
        }  

        strcpy(stuid,"1");  
        strcpy(subid,"1");  
        strcpy(id,stuid);  
        strcpy(id1,subid);  
        score = 98;  
        if((scores.select(q3) >0)&&(scores.select(q4) >0))  
        {  
            printf("this student's score of this subject is existed!\n");  
        }  
        else  
        {  
            if(students.select(q3) == 1)  
            {  
                sco.StuRef = students.currentId(); /* 可以引用到关联的记录，但是如果后面该关联的记录被删除，则这条记录也会引发错误 */  
            }  
            if(subjects.select(q4) ==1)  
            {  
                sco.SubRef = subjects.currentId();  
            }  
            sco.vc_stuid = stuid;  
            sco.vc_subid = subid;  
            sco.l_score = score;  
            insert(sco);  
            db.commit();  
            printf("score infomation insert successfully!\n");  
        }  

        if(students.select() >0)  
        {  
            do  
            {  
                printf("%s\t%s\t%s\t\n",students->vc_stuid,students->vc_stuname,students->vc_sex);  
            }while(students.next());  
        }  

        if(scores.select() > 0)  
        {  
            do  
            {  
                /* 如果students.at()不存在或者已经删除则会报错! */  
                printf("%s\t%s\t%d\t\n",students.at(scores->StuRef)->vc_stuname,subjects.at(scores->SubRef)->vc_subname,scores->l_score);  
            }while(scores.next());  
        }  
    }
    db.close();  
/*    if (Metatable.select() > 0 )  
    {  
        do  
        {  
            printf("name:%s,type:%s\n",Metatable.name,Metatable.type);  
        }while(Metatable.next());  
    } */ 
    getchar();
	return 0;
}

