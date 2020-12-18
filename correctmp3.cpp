#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include <algorithm> 
#include <vector>
#include<limits>

#include <bitset>
#include<typeinfo>

using namespace std;
string::size_type sz;

/* void readTraceFile(string tracefile)
{
    
    string Hex_Add;
    string type;
    string destn;
    string src1,src2;


    string line;
        
    ifstream myfile(tracefile);
    if (myfile.is_open())
    {
        while (getline(myfile,line))
        {
            istringstream iss(line);
			iss>>Hex_Add>>type>>destn>>src1>>src2;
            
            
                
        }
        myfile.close();
    }

}
*/

int S;
int N;
bool end_marker=false;
ifstream myfile;
int seq_num=0;
int instruction_count=0;



struct Instruction
{
    int Hex_Add;
    int type;
    int destn;
    int src1;
    int src2;
    string state;
    int tag;
    string S1_state;
    string S2_state;
    int fetch_start_cycle;
    int fetch_duration;
    int dispatch_start_cycle;;
    int dispatch_duration;
    int issue_start_cycle;
    int issue_duration;
    int execute_start_cycle;
    int execution_end_cycle;
    int execute_duration;
    int writeback_start_cycle;
    int writeback_duration;
    int src1_dependent;
    int src2_dependent;


    

};
typedef struct Instruction Instruction;

vector<Instruction> dispatch_list;
vector<Instruction> tempDispatch;
vector<Instruction> FakeROB;
vector<Instruction> ExecuteQueue;
vector<Instruction> SchedulingQueue;
vector<string> Register_state(128,"R");
vector<int> Register_tag(128,-1);
string tracefile;
unsigned int cycle_count=0;
int issue_count=0;
int execute_count=0;
int dispatch_count=0;

bool increasingOrderOfTags(Instruction i1,Instruction i2)
{
    return i1.tag<i2.tag;
}

bool Advance_Cycle()
{
    if(end_marker==true && SchedulingQueue.empty() && ExecuteQueue.empty() && dispatch_list.empty()) 
    {
        //cout<<"Adv cycle ended"<<endl;
        //cout<<"the final cycle count is"<<endl;
        return false;
    }
    else
    {
        cycle_count=cycle_count+1;
        //cout<<"Hi"<<endl;
        return true;
    }
    

}

/*void fakeRetire()
{
    //cout<<"Hi"<<endl;
    while(FakeROB.size()>0)
    {
        //cout<<"Hello"<<endl;

        vector<Instruction>::iterator iterROB = FakeROB.begin();
        if (iterROB->state == "WB")
        {
            //tempFakeROB.push_back()
            cout << dec << iterROB->tag << " fu{" << iterROB->type << "}"
                 << " src{" << iterROB->src1 << "," << iterROB->src2 << "}"
                 << " dst{" << iterROB->destn << "}"
                 << " IF{" << iterROB->fetch_start_cycle << "," << iterROB->fetch_duration<< "}"
                 << " ID{" << iterROB->dispatch_start_cycle << "," << iterROB->dispatch_duration << "}"
                 << " IS{" << iterROB->issue_start_cycle << "," << iterROB->issue_duration << "}"
                 << " EX{" << iterROB->execute_start_cycle << "," << iterROB->execution_end_cycle - iterROB->execute_start_cycle << "}"
                 << " WB{" << iterROB->writeback_start_cycle << "," << iterROB->writeback_duration << "}"
                 << "\n";
            FakeROB.erase(iterROB);
            //cout<<FakeROB[0].tag<<endl;
        }
        else
        {
            break;
        }
    }
}
*/

    
    
    
        
            
    

void execute()
{
    if(ExecuteQueue.size()>0)
    {

        vector<Instruction> tempExe;
        /*for(int i=0;i<ExecuteQueue.size();i++)
        {
            cout<<"Element in exe queue at pos "<<i<<"is "<<ExecuteQueue[i].tag<<endl;
        }
        */
        
        for(int i=0;i<ExecuteQueue.size();i++)
        {
            tempExe.push_back(ExecuteQueue[i]);
        }
        /*for(int i=0;i<tempExe.size();i++)
        {
            cout<<"Element in temp exe at pos "<<i<<"is "<<tempExe[i].tag<<endl;
        }
        */
        
        for(int i=0;i<ExecuteQueue.size();i++)
        {
            //cout<<"Execute queue size"<<ExecuteQueue.size()<<endl;
            int new1_tag=ExecuteQueue[i].tag;
            //cout<<"NEW1 TAG IS"<<ExecuteQueue[i].tag<<endl;
            //cout<<"TEMP EXE TAG IS "<<tempExe[i].tag<<endl;
            //cout<<"Execute tag is "<<new1_tag<<endl;
            //cout<<ExecuteQueue[1].tag<<endl;
            //Instruction new8;
            //new8=FakeROB.at(new1_tag);
            //cout<<"Cycle count at execution stage is"<<cycle_count<<endl;
            //cout<<"Expected cycle end of execution"<<FakeROB[new1_tag].execution_end_cycle;
            if(cycle_count==FakeROB[new1_tag].execution_end_cycle)
            {
                //cout<<"writeback tag"<<new1_tag<<endl;
                //cout<<"Executingggg"<<endl;
                //cout<<"current cycle is "<<cycle_count<<endl;
                FakeROB[new1_tag].state="WB";
                FakeROB[new1_tag].writeback_start_cycle=cycle_count;
                FakeROB[new1_tag].writeback_duration=1;
                //cout<<"I is "<<i<<endl;
                for(int j=0;j<tempExe.size();j++)
                {
                    if (new1_tag==tempExe[j].tag)
                    {
                        tempExe.erase(tempExe.begin()+j);
                    }
                }
                
                execute_count--;
                //cout<<"Execute Queue size after deleting"<<ExecuteQueue.size()<<endl;
            
                //cout<<"Temp exe queue size after deleting the element that has been executed is"<<tempExe.size()<<endl;
                //cout<<"CHECK"<<tempExe[0].tag<<endl;
                //cout<<tempExe[1].tag<<endl;
                if(FakeROB[new1_tag].destn!=-1)
                {
                    if(FakeROB[new1_tag].tag==Register_tag[FakeROB[new1_tag].destn])
                    {
                        //cout<<"Changed destination status to ready"<<endl;
                        Register_state[FakeROB[new1_tag].destn]="R";
                    }
                }
                for (int j=0;j<tempDispatch.size();j++)
                {
                    int disp_tag=tempDispatch[j].tag;
                    //Instruction new9;
                    //new9=FakeROB.at(disp_tag);

                    //cout<<new1_tag<<endl;
                    //cout<<disp_tag<<endl;
                    if(FakeROB[disp_tag].src1_dependent==FakeROB[new1_tag].tag)
                    {
                        FakeROB[disp_tag].S1_state="R";
                        //cout<<"SUCCESS"<<endl;
                    }
                    if(FakeROB[disp_tag].src2_dependent==FakeROB[new1_tag].tag)
                    {
                        FakeROB[disp_tag].S2_state="R";
                    }
                }
            }
            
            //cout<<"after deleting one element of execute queue, the next element present in exec queue is"<<ExecuteQueue[0].tag<<endl;
        }
        ExecuteQueue.clear();
        //cout<<"temp exe size after deleting"<<tempExe.size()<<endl;
        /*for(int i=0;i<tempExe.size();i++)
        {
            cout<<tempExe[i].tag<<endl;
        }
        */
        for(int i=0;i<tempExe.size();i++)
        {
            ExecuteQueue.push_back(tempExe[i]);
            
        }

        /*for(int i=0;i<ExecuteQueue.size();i++)
        {
            //cout<<"ELEMENTS STILL IN EXE QUEUE, WAITING FOR THEIR PARTICULAR CYCLE TO WB IS "<<ExecuteQueue[i].tag<<endl;
        }
        */
        
    }
}
    

    
        
        
void issue()
{
    if(SchedulingQueue.size()>0)
    {
        vector<Instruction> temp1;
        int latency;
        int final_count=0;
        for(int i=0;i<SchedulingQueue.size();i++)
        {
            int issue_tag=SchedulingQueue[i].tag;
            //cout<<"Issue tag is"<<issue_tag<<endl;
            //cout<<FakeROB[1].src1<<endl;
            //cout<<FakeROB[issue_tag].src1<<endl;
            //cout<<FakeROB[issue_tag].src2<<endl;
            //cout<<FakeROB[5].src1<<endl;
            //cout<<FakeROB[5].src2<<endl;

            //cout<<"SRC 1 REGISTER STATE "<<Register_state[FakeROB[issue_tag].src1]<<endl;
            //cout<<"SRC 2 REGISTER STATE "<<Register_state[FakeROB[issue_tag].src2]<<endl; 
            //Instruction new6;
            //new6=FakeROB.at(issue_tag);
            //cout<<FakeROB[issue_tag].S1_state<<endl;
            //cout<<FakeROB[issue_tag].S2_state<<endl;

            if(FakeROB[issue_tag].S1_state=="R" && FakeROB[issue_tag].S2_state=="R")
            {
                //cout<<"Push element inside"<<endl;
                temp1.push_back(FakeROB[issue_tag]);

            }
        }
        //cout<<"Size of temp1 Scheduling queue"<<temp1.size()<<endl;
        sort(temp1.begin(),temp1.end(),increasingOrderOfTags);
        if(temp1.size()<=N+1)
        {
            final_count=temp1.size();
        }
        else if(temp1.size()>N+1)
        {
            final_count=N+1;
        }
        
        for(int i=0;i<final_count;i++)
        {
            int ex_tag=temp1[i].tag;
            //cout<<"tag in issue stage"<<ex_tag<<endl;
            //Instruction new7;
            //new7=FakeROB.at(ex_tag);
        
            FakeROB[ex_tag].state="EX";
            FakeROB[ex_tag].execute_start_cycle=cycle_count;
            FakeROB[ex_tag].issue_duration=cycle_count-FakeROB[ex_tag].issue_start_cycle;
            ExecuteQueue.push_back(FakeROB[ex_tag]);
            //cout<<"Pushed tag into execute queue"<<FakeROB[ex_tag].tag<<endl;
            //cout<<ExecuteQueue[ex_tag].tag<<endl;
            //cout<<"After pushing into execute queue, the size now is"<<ExecuteQueue.size()<<endl;
            execute_count++;
            for(int j=0;j<SchedulingQueue.size();j++)
            {
                if(temp1[i].tag==SchedulingQueue[j].tag)
                {
                    //cout<<"After comparing "<<temp1[i].tag<<"with "<<SchedulingQueue[j].tag<<endl;
                    SchedulingQueue.erase(SchedulingQueue.begin()+j);
                    issue_count--;
                    //cout<<"Size of schedule queue after deletion"<<SchedulingQueue.size()<<endl;
                    break;
                }
            }
       

            //cout<<"Current cycle"<<cycle_count<<endl;
            //cout<<"ex tag is"<<ex_tag<<endl;
            if(FakeROB[ex_tag].type==0)
            
            {
                latency=1;
                FakeROB[ex_tag].execution_end_cycle=cycle_count+latency;
                //cout<<"latency is"<<latency<<endl;

            }
            else if(FakeROB[ex_tag].type==1)
            {
                latency=2;
                FakeROB[ex_tag].execution_end_cycle=cycle_count+latency;
                //cout<<"latency is"<<latency<<endl;
            }
            else if(FakeROB[ex_tag].type==2)
            {
                latency=5;
                FakeROB[ex_tag].execution_end_cycle=cycle_count+latency;
                //cout<<"latency is"<<latency<<endl;
            }
            //cout<<"Calculating end cycle execution at issue stage"<<FakeROB[ex_tag].execution_end_cycle<<endl;
        }

    }
}
    
void dispatch()
{
    if(dispatch_list.size()>0)
    {
        vector<Instruction> temp;
        //cout<<"Dispatch size at dispatch stage is"<<dispatch_list.size()<<endl;
        /*int size=0;
        int dispatch_size=dispatch_list.size();
        int available_space=S-issue_count;
        if(dispatch_size>=available_space)
        {
            size=available_space;

        }
        else if(dispatch_size<available_space)
        {
            size=dispatch_size;
        }
        */
        for(int i=0;i<dispatch_list.size();i++)
        {
            int tag1=dispatch_list[i].tag;
            //cout<<tag1<<endl;
            //Instruction new3;
            //new3=FakeROB.at(tag1);
            //cout<<FakeROB[i].state<<endl;
            //cout<<new3.state<<endl;
            if(FakeROB[tag1].state=="ID")
            {
                temp.push_back(FakeROB[tag1]);
            }
        }
        //cout<<" Pushing elements of ID state into temp,Temp dispatch size is"<<temp.size()<<endl;
    
        
    

        //vector<Instruction> SchedulingQueue;
        for(int i=0;i<temp.size();i++)
        {
            if(SchedulingQueue.size()<S)
            {
                int tag3=temp[i].tag;
                //Instruction new4;
                //new4=FakeROB.at(tag3);
            
            
                FakeROB[tag3].state="IS";
                FakeROB[tag3].issue_start_cycle=cycle_count;
                FakeROB[tag3].dispatch_duration=cycle_count-FakeROB[tag3].dispatch_start_cycle;
                SchedulingQueue.push_back(FakeROB[tag3]);
                //cout<<"size of scheduling queue after pushing elements of IS state is "<<SchedulingQueue.size()<<endl;
                issue_count++;


                //dispatch_list.erase(temp.begin()+0);
                //dispatch_count--;

                for(int j=0;j<dispatch_list.size();j++)
                {
                    if(temp[i].tag==dispatch_list[j].tag)
                    {
                        //cout<<"Tag erased from dispatch list is "<<dispatch_list[j].tag<<endl;;
                        dispatch_list.erase(dispatch_list.begin()+j);
                        //cout<<"After erasing, new dispatch size"<<dispatch_list.size()<<endl;
                        dispatch_count--;
                        break;
                    }
                    
                }
                
            }

        
        
            
            

            
        
            else
            {
                break;
            }
        
        }
        /*for(int i=0;i<dispatch_list.size();i++)
        {
            int tag4=dispatch_list[i].tag;
            //Instruction new5;
            //new5=FakeROB.at(tag4);
            
            if(FakeROB[tag4].state=="IS")
            {
                
                cout<<"tag to be deleted "<<tag4<<endl;
                cout<<"Before size"<<dispatch_list.size()<<endl;
                dispatch_list.erase(dispatch_list.begin()+i);
                cout<<"After erasing, new dispatch size"<<dispatch_list.size()<<endl;
                dispatch_count--;
            }

        }
        */
        
        
        
        for(int i=0;i<dispatch_list.size();i++)
        {
            //cout<<"size "<<dispatch_list.size()<<endl;
            int new_tag=dispatch_list[i].tag;
            //cout<<"New tag is"<<new_tag<<endl;
            //Instruction new2;
            //new2=FakeROB.at(new_tag);

            if(FakeROB[new_tag].state=="IF")
            {
                FakeROB[new_tag].state="ID";
                //cout<<"FakeROB (IF to ID) pos "<<new_tag<<" and its new state is "<<FakeROB[new_tag].state<<endl;
                //cout<<FakeROB[i].state<<endl;
                //cout<<"Register state at"<<new_tag<<"is "<<Register_state[new_tag]<<endl;
                //cout<<"Register state depending on src 1 of FakeROB "<<"at pos "<<new_tag<<"is "<<Register_state[FakeROB[new_tag].src1]<<endl;
                //cout<<"Register state depending on src 2 of FakeROB "<<"at pos "<<new_tag<<"is "<<Register_state[FakeROB[new_tag].src2]<<endl;
                if(FakeROB[new_tag].src1==-1)
                {
                    FakeROB[new_tag].S1_state="R";
                }
                else if(Register_state[FakeROB[new_tag].src1]=="R")
                {
                    //cout<<"HI"<<endl;
                    FakeROB[new_tag].S1_state=Register_state[FakeROB[new_tag].src1];
                }
                else if(Register_state[FakeROB[new_tag].src1]=="NR")
                {
                    FakeROB[new_tag].S1_state=Register_state[FakeROB[new_tag].src1];
                    FakeROB[new_tag].src1_dependent=Register_tag[FakeROB[new_tag].src1];
                    //cout<<"Src1 dependent"<<FakeROB[new_tag].src1_dependent<<endl;
                }
            
                if(FakeROB[new_tag].src2==-1)
                {
                    //cout<<"Hello"<<endl;
                    FakeROB[new_tag].S2_state="R";
                }
                else if(Register_state[FakeROB[new_tag].src2]=="R")
                {
                    FakeROB[new_tag].S2_state=Register_state[FakeROB[new_tag].src2];
                }
                else if(Register_state[FakeROB[new_tag].src2]=="NR")
                {
                    FakeROB[new_tag].S2_state=Register_state[FakeROB[new_tag].src2];
                    FakeROB[new_tag].src2_dependent=Register_tag[FakeROB[new_tag].src2];
                    //cout<<"Src2 dependent"<<FakeROB[new_tag].src2_dependent<<endl;

                }
            

                if(FakeROB[new_tag].destn!=-1)
                {
                    Register_tag[FakeROB[new_tag].destn]=FakeROB[new_tag].tag;
                    Register_state[FakeROB[new_tag].destn]="NR";
                }
                FakeROB[new_tag].dispatch_start_cycle=cycle_count;
                FakeROB[new_tag].fetch_duration=cycle_count-FakeROB[new_tag].fetch_start_cycle;
                //cout<<"src 1 of tag "<<new_tag<<"is "<<FakeROB[new_tag].S1_state<<endl;
                //cout<<"src 2 of tag "<<new_tag<<"is "<<FakeROB[new_tag].S2_state<<endl;
                
                

            }
        }
        
        
    }
    

    



}










void fetch()
{
    //cout<<tracefile<<endl;
    

    
    int fetch_bandwidth=0;
    

    
    
    
        
    //cout<<"Hello"<<endl;
    while(fetch_bandwidth<N && dispatch_list.size()<2*N)
    {
        //cout<<"Entered fetch while loop"<<endl;
        string line;
        //cout<<"Hi"<<endl;
        if (getline(myfile,line) && !line.empty())
        {
            string Hex_Add1;
            string type1;
            string destn1;
            string src11;
            string src21;

            
            //cout<<"enter"<<endl;
            istringstream iss(line);
            //cout<<line<<endl;
			iss>>Hex_Add1>>type1>>destn1>>src11>>src21;
            //cout<<Hex_Add1<<endl;
            //cout<<type1<<endl;
            //cout<<destn1<<endl;
            //cout<<src11<<endl;
            //cout<<src21<<endl;
            //cout<<typeid(Hex_Add1).name()<<endl;
            instruction_count++;
            Instruction new1;
            //cout<<typeid(new1).name()<<endl;
            new1.Hex_Add=stoi(Hex_Add1,&sz,16);
            new1.destn=stoi(destn1,&sz,10);
            new1.src1=stoi(src11,&sz,10);
            new1.src2=stoi(src21,&sz,10);
            new1.type=stoi(type1,&sz,10);
            new1.tag=seq_num;
            new1.state="IF";
                
            new1.fetch_start_cycle=cycle_count;
            seq_num++;
                
            FakeROB.push_back(new1);
            dispatch_list.push_back(new1);
            tempDispatch.push_back(new1);
            //tempFakeROB.push_back(new1);
            dispatch_count++;
        }
        else
        {
            
            
            //cout<<"File ended"<<endl;
            end_marker=true;
            myfile.close();
            
            break;
        }
        
        fetch_bandwidth++;
        //cout<<fetch_bandwidth<<endl;
        //cout<<dispatch_list.size()<<endl;
        
    }
    
    //cout<<dispatch_list.size()<<endl;
    //cout<<FakeROB.size()<<endl;
    //cout<<FakeROB[4].src1<<endl;

    

}
    
            

                

                
                

            
            
     

int main(int argc, char* argv[])
{
    S=atoi(argv[1]);
    N=atoi(argv[2]);
    tracefile=argv[3];
    
    myfile.open(tracefile);

    do
    {
        //fakeRetire();
        execute();
        issue();
        dispatch();
        fetch();
    } while(Advance_Cycle());

    for(int i=0;i<FakeROB.size();i++)
    {
        cout << dec << FakeROB[i].tag << " fu{" << FakeROB[i].type << "}"
                 << " src{" << FakeROB[i].src1 << "," << FakeROB[i].src2 << "}"
                 << " dst{" << FakeROB[i].destn << "}"
                 << " IF{" << FakeROB[i].fetch_start_cycle << "," << FakeROB[i].fetch_duration<< "}"
                 << " ID{" << FakeROB[i].dispatch_start_cycle << "," << FakeROB[i].dispatch_duration << "}"
                 << " IS{" << FakeROB[i].issue_start_cycle << "," << FakeROB[i].issue_duration << "}"
                 << " EX{" << FakeROB[i].execute_start_cycle << "," << FakeROB[i].execution_end_cycle - FakeROB[i].execute_start_cycle << "}"
                 << " WB{" << FakeROB[i].writeback_start_cycle << "," << FakeROB[i].writeback_duration << "}"
                 << "\n";
    }
    unsigned int total_cycle_count=cycle_count+1;

    cout <<"number of instructions = " << dec << instruction_count <<endl;
    cout<<"number of cycles        = " << dec << total_cycle_count<<endl;
    cout<<"IPC                     = " << fixed<<setprecision(5) << (float)instruction_count / total_cycle_count<<endl;
    

    return 0;
    
    
}

    

    
    



    



