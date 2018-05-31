#include<iostream>
#include<string>
#include<cstdlib>
#include<fstream>
#include<windows.h>
#include<time.h>
using namespace std;


struct Player{
	string firstname;
	string lastname;
	int runs;
	int oversBowled;
	bool bowledMax;
	int extrasBowled;
	bool out;
	bool onfield;
	bool bowled;
	string outReason;
	int wicketsTaken;
};

struct Team{
	Player players[11];
	int teamScore;
	int Extras;
};

//Global declarations
Team team1;
Team team2;
int numOvers;
int ballDelay;
int overDelay;
int inningDelay;
int playersOut=0;
string extra;
int overCount=1;
int primaryBatsman = 0;
int secondaryBatsman = 1;
int requiredRunChase=0;
ifstream nameFile;

//Function prototype declarations
void assignTeams();
int randBatsmanScore();
void Delay(int);
void Innings(Team&,Team&);
void Over(Team&,Team&,int);
bool isExtraBall();
int randBatsmanScore(Team&,int);
void swapBatsmen(int,int);
string randOut();
void reset();
bool runChaseSuccess(Team &batting,Team &bowling);
void scoreboard();

int main(){
	srand(time(NULL));
	
	cout<<"T-YY Cricket Competition Where You Specify the Amount of Overs"<<endl;
	cout<<"Please enter the amount of overs: ";
	cin>>numOvers;
	cout<<"Please specify the delay (in seconds) between balls: ";
	cin>>ballDelay;
	cout<<"Please specify the delay (in seconds) between overs: ";
	cin>>overDelay;
	cout<<"Please specify the delay (in seconds) between innings: ";
	cin>>inningDelay;
	cout<<endl;
	cout<<"T"<<numOvers<<" match about to begin!"<<endl;
	
	assignTeams();
	
	cout<<"Team 1 starts to bat..."<<endl;
	Innings(team1,team2);
	
	cout<<"Team 1 scored "<<team1.teamScore<<" runs"<<endl;
	cout<<"Team 2 must score "<<team1.teamScore +1<<" runs to win!"<<endl;
	cout<<"Team 2 starts to bat..."<<endl;
	cout<<"Team 2 is chasing "<<team1.teamScore+1<<" runs to win."<<endl<<endl;

	requiredRunChase = team1.teamScore +1;
	reset();
	Innings(team2,team1);
	if(team1.teamScore>team2.teamScore){
		cout<<"Team 1 has won! Team 1 has won!"<<endl;
	}
	else if(team2.teamScore>team1.teamScore){
		cout<<"Team 2 has won! Team 2 has won!"<<endl;
	}
	else{
		cout<<"Game is drawn!!!!"<<endl;
	}
	
	scoreboard();
}


void assignTeams(){            
	nameFile.open("Teams.txt");
	
	
	if(!nameFile.is_open()){
		cout<<"The file cannot be opened! Please ensure the file has no errors, exists or is in the program directory folder."<<endl;
		abort();
	}	
		
	int team1Count=0;
	int team2Count=0;
	int i = 0;		// count to keep track of how many players we are placing in the teams.
	
	while(i<22){
		
		if(i<11){
			nameFile>>team1.players[team1Count].firstname>>team1.players[team1Count].lastname;
			team1Count++;
			i++;
		}
		
		else if(i>=11 && i<=21){
			nameFile>>team2.players[team2Count].firstname>>team2.players[team2Count].lastname;
			team2Count++;
			i++;
		}
	}
	
	
	nameFile.close();
	
}

void Delay(int delayChoice){			//takes each choice of delay in seconds and changes to milliseconds so the Sleep function will work.
	
	int ms= delayChoice *1000;
	Sleep(ms);
}

void Innings(Team &batting, Team &bowling){
	int currentBowler;
	int previousBowler;
	
			//innings start at over 0 and ends at limit.
		//randomize bowler except a bowler cant bowl twice in a row and have a limit of overs
	while(overCount<=numOvers){			
		while(currentBowler==previousBowler || (bowling.players[currentBowler].bowledMax==true) ){		
			currentBowler = rand() % 12;			//while we keep randoming the same bowler previously, keep trying to get a new current bowler.
		}
		bowling.players[currentBowler].bowled=true;
		batting.players[primaryBatsman].onfield = true;
		batting.players[secondaryBatsman].onfield = true;
		Over(batting,bowling,currentBowler);			//invoke over function				//have to increment overcount after an over to forward the loop.
		previousBowler = currentBowler;				//to set previous bowler before we go thru the loops again.
		if(playersOut==10){
			break;
		}
		
		Delay(overDelay);							//delay after the over.
	}
	
	cout<<"End of innings."<<endl;	
	Delay(inningDelay);								//if the loops work, the num of overs should play out the an inning delay must be initiated.
					//put this at the begnning of second innings.
	
}


void Over(Team &batting, Team &bowling, int currentBowler){			//uses a bowler as a parameter, as well as both teams.
	int numBalls=1, outcome;
	int bowlingLimit;
	bowlingLimit = numOvers/5;

	while(numBalls<=6 && (!runChaseSuccess(batting,bowling)) ){
		if(isExtraBall()){
				 //even tho theres an extra delivery, the batsman can still play a shot therefore we need an outcome
			while(outcome>6 || outcome<0){
				outcome = randBatsmanScore(batting,primaryBatsman);		//this is needed because my function sometimes can return a negative number for specific reasons.
			}
			batting.players[primaryBatsman].runs+= outcome;		//let the outcome sum into the primary batter's runs.
			batting.teamScore += outcome;						//add into the teamscore total.
			batting.teamScore++;		//overall teamscore to be added later on
			bowling.players[currentBowler].extrasBowled += outcome+ 1;			//add up extras per bowler. (credited to extras of batting team)
			batting.Extras += outcome +1;
			cout<<"["<<bowling.players[currentBowler].lastname<<" to "<<batting.players[primaryBatsman].lastname<<"]";
			cout<<"<"<<overCount<<", "<<numBalls<<">";
			cout<<" "<<extra<<" "<<outcome<<" runs";
			cout<<" [Score : "<<batting.teamScore<<" for "<<playersOut<<"]";
			cout<<endl;
		}
		else{
			outcome = randBatsmanScore(batting,primaryBatsman);
			if(outcome== -1 ){
				cout<<"["<<bowling.players[currentBowler].lastname<<" to "<<batting.players[primaryBatsman].lastname<<"]";
				cout<<"<"<<overCount<<", "<<numBalls<<">";
				cout<<"OUT!!";
				cout<<" [Score : "<<batting.teamScore<<" for "<<playersOut<<"]";
				cout<<endl;
				numBalls++;
				bowling.players[currentBowler].wicketsTaken++;
				primaryBatsman++;
				if(batting.players[primaryBatsman].onfield==true){
					primaryBatsman++;										//if the next person on the field then increment once more to get the next batsman on team
				}
				batting.players[primaryBatsman].onfield=true;
				swap(primaryBatsman,secondaryBatsman);
				Delay(ballDelay);
				
			}
			else if(outcome== -2 ){			// a way to stack outs until 10 and then exit the innings.
				cout<<"["<<bowling.players[currentBowler].lastname<<" to "<<batting.players[primaryBatsman].lastname<<"]";
				cout<<"<"<<overCount<<", "<<numBalls<<">";
				cout<<"OUT!!";
				cout<<" [Score : "<<batting.teamScore<<" for "<<playersOut<<"]";
				cout<<endl;
				return;			//all players out on the team so return back into innings
			}
			else{
				//swap
			batting.players[primaryBatsman].runs += outcome;
			batting.teamScore += outcome;
			cout<<"["<<bowling.players[currentBowler].lastname<<" to "<<batting.players[primaryBatsman].lastname<<"]";
			cout<<"<"<<overCount<<", "<<numBalls<<">";
			cout<<" "<<outcome<<" runs";
			cout<<" [Score : "<<batting.teamScore<<" for "<<playersOut<<"]";
			cout<<endl;
			numBalls++;
			if(outcome==1 || outcome==3){
					swap(primaryBatsman,secondaryBatsman);
			}
			Delay(ballDelay);
			}
			
		}
	}
	cout<<" --> End of over #"<<overCount<<endl;
	overCount++;
	swap(primaryBatsman,secondaryBatsman);
	bowling.players[currentBowler].oversBowled++;
	if(bowling.players[currentBowler].oversBowled>=bowlingLimit){
		bowling.players[currentBowler].bowledMax=true;
	}
	cout<<bowling.players[currentBowler].lastname<<" has bowled "<<bowling.players[currentBowler].oversBowled<<" overs."<<endl;
}

bool isExtraBall(){			
	
	int weight, score;
	 weight = rand() % 101;
		if(weight<=5){					
			extra = "No Ball!";
			return true;
			
			//no ball.
		}
		else if(weight>5 && weight<=8 ){
			extra = "Wide!";
			return true;		
		}
		else return false;
}

int randBatsmanScore(Team &battingTeam,int currentBatsman){				
	int weight;
	
	weight = rand() % 101;
		if(weight>=0 && weight<=17){
			return 6;
		}
		if(weight>17 && weight<=20){
			return 5;
		}
		if(weight>20 && weight<=40){
			return 4;
		}
		if(weight>40 && weight<=45){
			return  3;
		}
		if(weight>45 && weight<=60){
			return 2;
		}
		if(weight>60 && weight<=75){
			return 1;
		}
		if(weight>75 && weight<=80){
			return 0;
		}
		if(weight> 80 && weight<=100){
			battingTeam.players[currentBatsman].out = true;
			playersOut++;			//batsman out
			if(playersOut==10){
				return -2;
			}
			else{
				return -1;
			}
		}
}

void swapBatsmen(int primary, int secondary){
	int temp;
	temp = primary;
	primary = secondary;
	secondary = temp;
}

string randOut(){
	int weight;
	
	weight = (rand() % 5 + 1);
	if(weight == 1){
		return "Clean Bowled";
	}
	if(weight == 2){
		return "Caught";
	}
	if(weight == 3){
		return "LBW";
	}
	if(weight == 4){
		return "Hit wicket";
	}
	if(weight == 5){
		return "Stumped";
	}
}

void reset(){
	overCount=1;
	primaryBatsman = 0;
	secondaryBatsman = 1;
	playersOut=0;
}

bool runChaseSuccess(Team &batting,Team &bowling){
	if(batting.teamScore>requiredRunChase && requiredRunChase!=0){
		return true;
	}
	else return false;
}

void scoreboard(){
	cout<<"		--------[SCOREBOARD]--------"<<endl<<endl;
	cout<<"Team 1 Batting:"<<endl<<endl;
	
	for(int i=0;i<11;i++){							//team 1
		if(team1.players[i].onfield==true){
			if(team1.players[i].out==true){
				team1.players[i].outReason = randOut();
				cout<<team1.players[i].firstname<<" "<<team1.players[i].lastname<<" "<<team1.players[i].outReason<<" "<<team1.players[i].runs<<endl;
			}
			else{
				cout<<team1.players[i].firstname<<" "<<team1.players[i].lastname<<" Not Out "<<team1.players[i].runs<<endl;
			}
		}

	}
	cout<<endl<<"Extras: "<<team1.Extras<<endl;
	cout<<endl;
	cout<<"Team 2 Bowling:"<<endl;
	cout<<endl;
	for(int j=0;j<11;j++){
		if(team2.players[j].bowled==true){
			cout<<team2.players[j].firstname<<" "<<team2.players[j].lastname<<" "<<team2.players[j].oversBowled<<", "<<team2.players[j].wicketsTaken<<" wickets ";
			cout<<team2.players[j].extrasBowled<<" extras bowled"<<endl;
		}
	}
	cout<<endl;
	cout<<"Team 2 Batting:"<<endl<<endl;
	
	for(int k=0;k<11;k++){							//team 1
		if(team2.players[k].onfield==true){
			if(team2.players[k].out==true){
				team2.players[k].outReason = randOut();
				cout<<team2.players[k].firstname<<" "<<team2.players[k].lastname<<" "<<team2.players[k].outReason<<" "<<team2.players[k].runs<<endl;
			}
			else{
				cout<<team2.players[k].firstname<<" "<<team2.players[k].lastname<<" Not Out "<<team2.players[k].runs<<endl;
			}
		}
	}
	cout<<endl<<"Extras: "<<team2.Extras<<endl;
	cout<<endl;
	cout<<"Team 1 Bowling:"<<endl;
	
	for(int l=0;l<11;l++){
		if(team1.players[l].bowled==true){
			cout<<team1.players[l].firstname<<" "<<team1.players[l].lastname<<" "<<team1.players[l].oversBowled<<", "<<team1.players[l].wicketsTaken<<" wickets ";
			cout<<team1.players[l].extrasBowled<<" extras"<<endl;
		}
	}
}











