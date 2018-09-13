#include <iostream>
#include <algorithm>
#include <functional>
#include <vector>
#include <iomanip>
#include <string>
#include <sstream>
#include <cstring>
#include <unordered_map>
#include <queue>
#include <set>
#include "getopt.h"
using namespace std;

struct equity
{
    int ID;
    int timestamp;
    string client_name;
    bool buy_or_sell; //buy=1, sell=0;
    string symbol;
    int price;
    int quantity;
    int duration;
};
void printequity(equity* a)
{
    cout<<a->ID<<" "<<a->timestamp<<" "<<a->client_name<<" "<<a->buy_or_sell<<" "<<a->symbol<<" "<<a->price<<" "<<a->quantity<<" "<<a->duration<<endl;
}
struct compare_sell
{
    bool operator()(equity* a, equity* b) const
    {
        if (a->price>b->price) return 1;
        else if (a->price<b->price) return 0;
        else
        {
            if (a->ID>b->ID) return 1;
            else return 0;
        }
    }
};
struct compare_buy
{
    bool operator()(equity* a, equity* b) const
    {
        if (a->price<b->price) return 1;
        else if (a->price>b->price) return 0;
        else
        {
            if (a->ID>b->ID) return 1;
            else return 0;
        }
    }
};
struct equity_and_money
{
    string equityname;
    int money;
};
struct client_information
{
    string client;
    int buy_quantity;
    int sell_quantity;
    int earned_money;
};
struct equity_compare
{
    bool operator()(equity_and_money* a,equity_and_money* b) const
    {
        return a->equityname.compare(b->equityname)>0;
    }
};
struct client_compare
{
    bool operator()(client_information* a, client_information* b) const
    {
        return a->client.compare(b->client)>0;
    }
};
struct time_travel_info
{
    bool have_trade;
    bool have_sell;
    string tttname;
    int buy_time; //someone sells
    int buy_money;
    int sell_time; //someone buys
    int sell_money;
    int buy_money2;
    int buy_time2;
};
void check_time_duration(priority_queue<equity*,vector<equity*>,compare_buy>* a,int time_difference)
{
    if (a->empty()) return;
    equity* item=new equity(*(a->top()));
    a->pop();
    check_time_duration(a,time_difference);
    if (item->duration!=-1)
    {
        if (item->duration<=time_difference) delete item;
        else
        {
            item->duration -= time_difference;
            a->push(item);
        }
    }
    else a->push(item);
}
void check_time_duration(priority_queue<equity*,vector<equity*>,compare_sell>* a,int time_difference)
{
    if (a->empty()) return;
    equity* item=new equity(*(a->top()));
    a->pop();
    check_time_duration(a,time_difference);
    if (item->duration!=-1)
    {
        if (item->duration<=time_difference) delete item;
        else
        {
            item->duration -= time_difference;
            a->push(item);
        }
    }
    else a->push(item);
}
void sortit(vector<int> *a)
{
    int i=a->size();
    while (i!=1 && (*a)[i-1]<(*a)[i-2])
    {
        int victim=(*a)[i-1];
        (*a)[i-1]=(*a)[i-2];
        (*a)[i-2]=victim;
        i--;
    }
}
void updatevalue(priority_queue<equity_and_money*,vector<equity_and_money*>,equity_compare> &a, equity_and_money* b)
{
    if (a.empty())
    {
        a.push(b);
        return;
    }
    equity_and_money* test=new equity_and_money(*(a.top()));
    a.pop();
    if (test->equityname.compare(b->equityname)==0)
    {
        delete test;
        a.push(b);
    }
    else
    {
        updatevalue(a,b);
        a.push(test);
    }
}
void printmedian(priority_queue<equity_and_money*,vector<equity_and_money*>,equity_compare> &a,int b)
{
    if (a.empty()) return;
    equity_and_money* baz=new equity_and_money(*(a.top()));
    a.pop();
    cout<<"Median match price of "<<baz->equityname<<" at time "<<b<<" is $"<<baz->money<<endl;
    printmedian(a,b);
    a.push(baz);
}
void printmidpoint(priority_queue<equity_and_money*,vector<equity_and_money*>,equity_compare> &a,int b)
{
    if (a.empty()) return;
    equity_and_money* ba=new equity_and_money(*(a.top()));
    a.pop();
    if (ba->money!=-1) cout<<"Midpoint of "<<ba->equityname<<" at time "<<b<<" is $"<<ba->money<<endl;
    else cout<<"Midpoint of "<<ba->equityname<<" at time "<<b<<" is undefined"<<endl;
    printmidpoint(a,b);
    delete ba;
}
void printclientinfo(priority_queue<client_information*,vector<client_information*>,client_compare> a)
{
    if (a.empty()) return;
    client_information* bar=new client_information(*(a.top()));
    a.pop();
    cout<<bar->client<<" bought "<<bar->buy_quantity<<" and sold "<<bar->sell_quantity<<" for a net transfer of $"<<bar->earned_money<<endl;
    printclientinfo(a);
    delete bar;
}
struct option long_options[] = {
    { "v", 0, NULL, 'v' },
    { "verbose", 0, NULL, 'v' },
    { "m", 0, NULL, 'm' },
    { "median", 0, NULL, 'm' },
    { "p", 0, NULL, 'p' },
    { "midpoint", 0, NULL, 'p' },
    { "t", 0, NULL, 't' },
    { "transfers", 0, NULL, 't' },
    { "g", 1, NULL, 'g' },
    { "ttt", 1, NULL, 'g' },
    { 0, 0, 0, 0},
};
int main(int argc, char *argv[])
{
    int a;
    int id=0;
    int current_timestamp=0;
    int commision_earning=0;
    int money_transfered=0;
    int completed_trades=0;
    int traded_shares=0;
    bool verbose;
    bool median;
    bool midpoint;
    bool transfers;
    int choosecommand;
    unordered_map<string,int> name;
    unordered_map<string,int>::const_iterator got;
    unordered_map<string,int> clientname;
    unordered_map<string,int>::const_iterator client_got;
    unordered_map<string,int>::const_iterator client_got2;
    unordered_map<string,int> time_travel;
    unordered_map<string,int>::const_iterator ttt_got;
    vector<priority_queue<equity*,vector<equity*>,compare_buy>*> buying;
    vector<priority_queue<equity*,vector<equity*>,compare_sell>*> selling;
    vector<vector<int>*> equity_trade;
    priority_queue<equity_and_money*,vector<equity_and_money*>,equity_compare> equitymedian;
    priority_queue<equity_and_money*,vector<equity_and_money*>,equity_compare> equitymidpoint;
    vector<client_information*> clientclient;
    vector<time_travel_info*> ttt;
    vector<int> foo;
    priority_queue<client_information*,vector<client_information*>,client_compare> clientinfo;
    while((choosecommand = getopt_long(argc, argv, "vmptg:", long_options, NULL)) != -1)
    {
        switch (choosecommand)
        {
            case 'v':
            {
                verbose=1;
                break;
            }
            case 'm':
            {
                median=1;
                break;
            }
            case 'p':
            {
                midpoint=1;
                break;
            }
            case 't':
            {
                transfers=1;
                break;
            }
            case 'g':
            {
                string s1=optarg;
                ttt_got=time_travel.find(s1);
                if (ttt_got==time_travel.end())
                {
                    pair<string,int> hashing3(s1,time_travel.size());
                    time_travel.insert(hashing3);
                    time_travel_info* qqq=new time_travel_info;
                    qqq->buy_money=0;
                    qqq->buy_time=0;
                    qqq->have_sell=0;
                    qqq->have_trade=0;
                    qqq->sell_money=0;
                    qqq->sell_time=0;
                    qqq->buy_money2=0;
                    qqq->buy_time2=0;
                    qqq->tttname=s1;
                    ttt.push_back(qqq);
                }
                break;
            }
        }
    }
    while (cin>>a)
    {
        if (a!=current_timestamp)
        {
            if (median)
            {
                printmedian(equitymedian,current_timestamp);
            }
            if (midpoint)
            {
                got=name.begin();
                while (got!=name.end())
                //for (int i=buying.size()-1;i>=0;i--)
                //for (int i=0;i<buying.size();i++)
                {
                    int i=got->second;
                    if (buying[i]->empty()==1 || selling[i]->empty()==1)
                    {
                        equity_and_money* foos=new equity_and_money;
                        foos->equityname=got->first;
                        foos->money=-1;
                        equitymidpoint.push(foos);
                    }
                    else
                    {
                        equity_and_money* foos=new equity_and_money;
                        foos->equityname=got->first;
                        equity* buy=new equity(*(buying[i]->top()));
                        equity* sell=new equity(*(selling[i]->top()));
                        //buy=buying[i]->top();
                        //sell=selling[i]->top();
                        foos->money=(buy->price+sell->price)/2;
                        equitymidpoint.push(foos);
                        delete buy;
                        delete sell;
                    }
                    got++;
                }
                printmidpoint(equitymidpoint,current_timestamp);
            }
            int time_difference=a-current_timestamp;
            for (int i=0;i<buying.size();i++)
            {
                priority_queue<equity*,vector<equity*>,compare_buy>* a=buying[i];
                check_time_duration(a,time_difference);
            }
            for (int i=0;i<selling.size();i++)
            {
                priority_queue<equity*,vector<equity*>,compare_sell>* b=selling[i];
                check_time_duration(b,time_difference);
            }
            current_timestamp=a;
        }
        equity* gupiao=new equity;
        gupiao->ID=id;
        gupiao->timestamp=a;
        cin>>gupiao->client_name;
        string buy_sell;
        cin>>buy_sell;
        if (buy_sell.compare("BUY")==0) gupiao->buy_or_sell=1;
        else gupiao->buy_or_sell=0;
        cin>>gupiao->symbol;
        char victim;
        cin>>victim;
        cin>>gupiao->price;
        cin>>victim;
        cin>>gupiao->quantity;
        cin>>gupiao->duration;
        got=name.find(gupiao->symbol);
        client_got=clientname.find(gupiao->client_name);
        ttt_got=time_travel.find(gupiao->symbol);
        if (ttt_got!=time_travel.end())
        {
            int ttt_index=ttt_got->second;
            if (gupiao->buy_or_sell==0)
            {
                if (ttt[ttt_index]->have_sell==0)
                {
                    ttt[ttt_index]->have_sell=1;
                    ttt[ttt_index]->buy_time=gupiao->timestamp;
                    ttt[ttt_index]->buy_money=gupiao->price;
                }
                else
                {
                    if (ttt[ttt_index]->have_trade==0)
                    {
                        if (ttt[ttt_index]->buy_money>gupiao->price)
                        {
                            ttt[ttt_index]->buy_money=gupiao->price;
                            ttt[ttt_index]->buy_time=gupiao->timestamp;
                        }
                    }
                    else
                    {
                        if (ttt[ttt_index]->buy_money>gupiao->price)
                        {
                            ttt[ttt_index]->buy_money2=gupiao->price;
                            ttt[ttt_index]->buy_time2=gupiao->timestamp;
                        }
                    }
                }
            }
            else
            {
                if (ttt[ttt_index]->have_sell==1)
                {
                    if (ttt[ttt_index]->have_trade==0)
                    {
                        ttt[ttt_index]->have_trade=1;
                        ttt[ttt_index]->sell_money=gupiao->price;
                        ttt[ttt_index]->sell_time=gupiao->timestamp;
                    }
                    else
                    {
                        if (ttt[ttt_index]->buy_money2==0)
                        {
                            if (ttt[ttt_index]->sell_money<gupiao->price)
                            {
                                ttt[ttt_index]->sell_money=gupiao->price;
                                ttt[ttt_index]->sell_time=gupiao->timestamp;
                            }
                        }
                        else
                        {
                            if (ttt[ttt_index]->sell_money<gupiao->price)
                            {
                                ttt[ttt_index]->buy_money=ttt[ttt_index]->buy_money2;
                                ttt[ttt_index]->buy_time=ttt[ttt_index]->buy_time2;
                                ttt[ttt_index]->buy_money2=0;
                                ttt[ttt_index]->buy_time2=0;
                                ttt[ttt_index]->sell_money=gupiao->price;
                                ttt[ttt_index]->sell_time=gupiao->timestamp;
                            }
                        }
                    }
                }
            }
        }
        if (got==name.end())
        {
            pair<string,int> hashing(gupiao->symbol,name.size());
            name.insert(hashing);
            priority_queue<equity*,vector<equity*>,compare_buy>* vec1=new priority_queue<equity*,vector<equity*>,compare_buy>;
            buying.push_back(vec1);
            priority_queue<equity*,vector<equity*>,compare_sell>* vec2=new priority_queue<equity*,vector<equity*>,compare_sell>;
            selling.push_back(vec2);
            vector<int>* ptr=new vector<int>;
            equity_trade.push_back(ptr);
        }
        if (client_got==clientname.end())
        {
            pair<string,int> hashing2(gupiao->client_name,clientname.size());
            clientname.insert(hashing2);
            client_information* cc=new client_information;
            cc->client=gupiao->client_name;
            cc->buy_quantity=0;
            cc->earned_money=0;
            cc->sell_quantity=0;
            clientclient.push_back(cc);
        }
        got=name.find(gupiao->symbol);
        client_got=clientname.find(gupiao->client_name);
        int index=got->second;
        int client_index=client_got->second;
        if (gupiao->buy_or_sell==1) //buying
        {
            here:
            if (selling[index]->empty()==1)
            {
                if (gupiao->duration!=0) buying[index]->push(gupiao);
                else delete gupiao;
            }
            else
            {
                equity* sell_min=selling[index]->top();
                selling[index]->pop();
                if (sell_min->price>gupiao->price)
                {
                    selling[index]->push(sell_min);
                    if (gupiao->duration!=0) buying[index]->push(gupiao);
                    else delete gupiao;
                }
                else
                {
                    client_got2=clientname.find(sell_min->client_name);
                    int client_index2=client_got2->second;
                    if (sell_min->quantity>gupiao->quantity)
                    {
                        if (verbose)
                        cout<<gupiao->client_name<<" purchased "<<gupiao->quantity<<" shares of "<<gupiao->symbol<<" from "<<sell_min->client_name<<" for $"<<sell_min->price<<"/share"<<endl;
                        int one_side_commision_fee=sell_min->price*gupiao->quantity/100;
                        commision_earning += 2*one_side_commision_fee;
                        money_transfered += sell_min->price*gupiao->quantity;
                        completed_trades++;
                        equity_trade[index]->push_back(sell_min->price);
                        traded_shares += gupiao->quantity;
                        sortit(equity_trade[index]);
                        int mediannumber;
                        foo=*equity_trade[index];
                        if (foo.size() % 2==0)
                        {
                            mediannumber=(foo[foo.size()/2]+foo[foo.size()/2-1])/2;
                        }
                        else mediannumber=foo[(foo.size()-1)/2];
                        equity_and_money* bar=new equity_and_money;
                        bar->equityname=gupiao->symbol;
                        bar->money=mediannumber;
                        updatevalue(equitymedian,bar);
                        clientclient[client_index]->buy_quantity += gupiao->quantity;
                        clientclient[client_index]->earned_money -= sell_min->price*gupiao->quantity;
                        clientclient[client_index2]->sell_quantity += gupiao->quantity;
                        clientclient[client_index2]->earned_money += sell_min->price*gupiao->quantity;
                        sell_min->quantity -=gupiao->quantity;
                        selling[index]->push(sell_min);
                        delete gupiao;
                    }
                    else if (sell_min->quantity<gupiao->quantity)
                    {
                        if (verbose)
                        cout<<gupiao->client_name<<" purchased "<<sell_min->quantity<<" shares of "<<gupiao->symbol<<" from "<<sell_min->client_name<<" for $"<<sell_min->price<<"/share"<<endl;
                        equity_trade[index]->push_back(sell_min->price);
                        int one_side_commision_fee=sell_min->price*sell_min->quantity/100;
                        commision_earning += 2*one_side_commision_fee;
                        money_transfered += sell_min->price*sell_min->quantity;
                        completed_trades++;
                        traded_shares += sell_min->quantity;
                        sortit(equity_trade[index]);
                        int mediannumber;
                        foo=*equity_trade[index];
                        if (foo.size() % 2==0)
                        {
                            mediannumber=(foo[foo.size()/2]+foo[foo.size()/2-1])/2;
                        }
                        else mediannumber=foo[(foo.size()-1)/2];
                        equity_and_money* bar=new equity_and_money;
                        bar->equityname=gupiao->symbol;
                        bar->money=mediannumber;
                        updatevalue(equitymedian,bar);
                        clientclient[client_index]->buy_quantity += sell_min->quantity;
                        clientclient[client_index]->earned_money -= sell_min->price*sell_min->quantity;
                        clientclient[client_index2]->sell_quantity += sell_min->quantity;
                        clientclient[client_index2]->earned_money += sell_min->price*sell_min->quantity;
                        gupiao->quantity -= sell_min->quantity;
                        delete sell_min;
                        goto here;
                    }
                    else
                    {
                        if (verbose)
                        cout<<gupiao->client_name<<" purchased "<<sell_min->quantity<<" shares of "<<gupiao->symbol<<" from "<<sell_min->client_name<<" for $"<<sell_min->price<<"/share"<<endl;
                        equity_trade[index]->push_back(sell_min->price);
                        sortit(equity_trade[index]);
                        int mediannumber;
                        foo=*equity_trade[index];
                        if (foo.size() % 2==0)
                        {
                            mediannumber=(foo[foo.size()/2]+foo[foo.size()/2-1])/2;
                        }
                        else mediannumber=foo[(foo.size()-1)/2];
                        equity_and_money* bar=new equity_and_money;
                        bar->equityname=gupiao->symbol;
                        bar->money=mediannumber;
                        updatevalue(equitymedian,bar);
                        int one_side_commision_fee=sell_min->price*sell_min->quantity/100;
                        commision_earning += 2*one_side_commision_fee;
                        money_transfered += sell_min->price*sell_min->quantity;
                        completed_trades++;
                        traded_shares += sell_min->quantity;
                        clientclient[client_index]->buy_quantity += sell_min->quantity;
                        clientclient[client_index]->earned_money -= sell_min->price*sell_min->quantity;
                        clientclient[client_index2]->sell_quantity += sell_min->quantity;
                        clientclient[client_index2]->earned_money += sell_min->price*sell_min->quantity;
                        gupiao->quantity -= sell_min->quantity;
                        delete gupiao;
                        delete sell_min;
                    }
                }
            }
        }
        else //selling
        {
            there:
            if (buying[index]->empty()==1)
            {
                if (gupiao->duration!=0) selling[index]->push(gupiao);
                else delete gupiao;
            }
            else
            {
                equity* buy_max=buying[index]->top();
                buying[index]->pop();
                if (buy_max->price<gupiao->price)
                {
                    buying[index]->push(buy_max);
                    if (gupiao->duration!=0) selling[index]->push(gupiao);
                    else delete gupiao;
                }
                else
                {
                    client_got2=clientname.find(buy_max->client_name);
                    int client_index2=client_got2->second;
                    if (buy_max->quantity>gupiao->quantity)
                    {
                        if (verbose)
                        cout<<buy_max->client_name<<" purchased "<<gupiao->quantity<<" shares of "<<gupiao->symbol<<" from "<<gupiao->client_name<<" for $"<<buy_max->price<<"/share"<<endl;
                        equity_trade[index]->push_back(buy_max->price);
                        int one_side_commision_fee=buy_max->price*gupiao->quantity/100;
                        commision_earning += 2*one_side_commision_fee;
                        money_transfered += buy_max->price*gupiao->quantity;
                        completed_trades++;
                        traded_shares += gupiao->quantity;
                        sortit(equity_trade[index]);
                        int mediannumber;
                        foo=*equity_trade[index];
                        if (foo.size() % 2==0)
                        {
                            mediannumber=(foo[foo.size()/2]+foo[foo.size()/2-1])/2;
                        }
                        else mediannumber=foo[(foo.size()-1)/2];
                        equity_and_money* bar=new equity_and_money;
                        bar->equityname=gupiao->symbol;
                        bar->money=mediannumber;
                        updatevalue(equitymedian,bar);
                        clientclient[client_index]->sell_quantity += gupiao->quantity;
                        clientclient[client_index]->earned_money += buy_max->price*gupiao->quantity;
                        clientclient[client_index2]->buy_quantity += gupiao->quantity;
                        clientclient[client_index2]->earned_money -= buy_max->price*gupiao->quantity;
                        buy_max->quantity -=gupiao->quantity;
                        buying[index]->push(buy_max);
                        delete gupiao;
                    }
                    else if (buy_max->quantity<gupiao->quantity)
                    {
                        if (verbose)
                        cout<<buy_max->client_name<<" purchased "<<buy_max->quantity<<" shares of "<<gupiao->symbol<<" from "<<gupiao->client_name<<" for $"<<buy_max->price<<"/share"<<endl;
                        equity_trade[index]->push_back(buy_max->price);
                        int one_side_commision_fee=buy_max->price*buy_max->quantity/100;
                        commision_earning += 2*one_side_commision_fee;
                        money_transfered += buy_max->price*buy_max->quantity;
                        completed_trades++;
                        traded_shares += buy_max->quantity;
                        sortit(equity_trade[index]);
                        int mediannumber;
                        foo=*equity_trade[index];
                        if (foo.size() % 2==0)
                        {
                            mediannumber=(foo[foo.size()/2]+foo[foo.size()/2-1])/2;
                        }
                        else mediannumber=foo[(foo.size()-1)/2];
                        equity_and_money* bar=new equity_and_money;
                        bar->equityname=gupiao->symbol;
                        bar->money=mediannumber;
                        updatevalue(equitymedian,bar);
                        clientclient[client_index]->sell_quantity += buy_max->quantity;
                        clientclient[client_index]->earned_money += buy_max->price*buy_max->quantity;
                        clientclient[client_index2]->buy_quantity += buy_max->quantity;
                        clientclient[client_index2]->earned_money -= buy_max->price*buy_max->quantity;
                        gupiao->quantity -= buy_max->quantity;
                        delete buy_max;
                        goto there;
                    }
                    else
                    {
                        if (verbose)
                        cout<<buy_max->client_name<<" purchased "<<buy_max->quantity<<" shares of "<<gupiao->symbol<<" from "<<gupiao->client_name<<" for $"<<buy_max->price<<"/share"<<endl;
                        equity_trade[index]->push_back(buy_max->price);
                        int one_side_commision_fee=buy_max->price*buy_max->quantity/100;
                        commision_earning += 2*one_side_commision_fee;
                        money_transfered += buy_max->price*buy_max->quantity;
                        completed_trades++;
                        traded_shares += buy_max->quantity;
                        sortit(equity_trade[index]);
                        int mediannumber;
                        foo=*equity_trade[index];
                        if (foo.size() % 2==0)
                        {
                            mediannumber=(foo[foo.size()/2]+foo[foo.size()/2-1])/2;
                        }
                        else mediannumber=foo[(foo.size()-1)/2];
                        equity_and_money* bar=new equity_and_money;
                        bar->equityname=gupiao->symbol;
                        bar->money=mediannumber;
                        updatevalue(equitymedian,bar);
                        clientclient[client_index]->sell_quantity += buy_max->quantity;
                        clientclient[client_index]->earned_money += buy_max->price*buy_max->quantity;
                        clientclient[client_index2]->buy_quantity += buy_max->quantity;
                        clientclient[client_index2]->earned_money -= buy_max->price*buy_max->quantity;
                        delete buy_max;
                        delete gupiao;
                    }
                }
            }
        }
        id++;
    }
    if (median)
        {
            printmedian(equitymedian,current_timestamp);
        }
        if (midpoint)
        {
                got=name.begin();
                while (got!=name.end())
                //for (int i=buying.size()-1;i>=0;i--)
                //for (int i=0;i<buying.size();i++)
                {
                    int i=got->second;
                    if (buying[i]->empty()==1 || selling[i]->empty()==1)
                    {
                        equity_and_money* foos=new equity_and_money;
                        foos->equityname=got->first;
                        foos->money=-1;
                        equitymidpoint.push(foos);
                    }
                    else
                    {
                        equity_and_money* foos=new equity_and_money;
                        foos->equityname=got->first;
                        equity* buy=new equity(*(buying[i]->top()));
                        equity* sell=new equity(*(selling[i]->top()));
                        //buy=buying[i]->top();
                        //sell=selling[i]->top();
                        foos->money=(buy->price+sell->price)/2;
                        equitymidpoint.push(foos);
                        delete buy;
                        delete sell;
                    }
                    got++;
                }
                printmidpoint(equitymidpoint,current_timestamp);
        }
    cout<<"---End of Day---"<<endl;
    cout<<"Commission Earnings: $"<<commision_earning<<endl;
    cout<<"Total Amount of Money Transferred: $"<<money_transfered<<endl;
    cout<<"Number of Completed Trades: "<<completed_trades<<endl;
    cout<<"Number of Shares Traded: "<<traded_shares<<endl;
    if (transfers)
    {
        for (int i=0;i<clientclient.size();i++)
        {
            clientinfo.push(clientclient[i]);
        }
        printclientinfo(clientinfo);
    }
    if (ttt.empty()==0)
    {
        for (int i=0;i<ttt.size();i++)
        {
            if (ttt[i]->have_trade==1) cout<<"Time travelers would buy "<<ttt[i]->tttname<<" at time: "<<ttt[i]->buy_time<<" and sell it at time: "<<ttt[i]->sell_time<<endl;
            else cout<<"Time travelers would buy "<<ttt[i]->tttname<<" at time: -1 and sell it at time: -1"<<endl;
        }
    }
}
