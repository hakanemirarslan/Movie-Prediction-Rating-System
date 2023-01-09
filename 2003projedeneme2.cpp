#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <map>
#include <unordered_map>
#include <sstream>
#include <algorithm>

using namespace std;

struct user{
int userid , n;
};

bool CompareUsers(user u1, user u2){
return u1.n>u2.n;
}

struct item{
int itemid,n;
};

bool CompareItem(item i1, item i2) {
  return i1.n>i2.n;
}

class Prediction {
private:
  unordered_map<int, map<int, float>> Users; 
  unordered_map<int, map<int, float>> Items; 
public:
  Prediction(const string& trainpath, const string& testpath, const string& resultpath) { // BU YER MERTİN KODU BENİMKİ BÖYLE DEĞİL
    ReadTrainData(trainpath);
    WriteResults(testpath, resultpath);
  }


  float Cosine(vector<float>& v1, vector<float>& v2) {
    float res = 0, mag1 = 0, mag2 = 0;
    for (int i = 0; i < v1.size(); i++) {
      res += v1[i] * v2[i];
      mag1 += v1[i] * v1[i];
      mag2 += v2[i] * v2[i];
    }
    mag1 = sqrt(mag1);
    mag2 = sqrt(mag2);
    return res / (sqrt(mag1) * sqrt(mag2));
  }

  void ReadTrainData(const string& trainpath) {
    ifstream train;
    train.open(trainpath);
    string data;
    getline(train, data);
    while (getline(train, data)) {
      stringstream ss(data);
      string user_id, item_id, rating;
      getline(ss, user_id, ','); // untill sees the first comma symbol it reads the column and initilaze the data as userid
      getline(ss, item_id, ','); // untill sees the second comma symbol it reads the column and initilaze the data as itemid
      getline(ss, rating);      // untill the rest of the column and initilaze the data as rating
      Users[stoi(user_id)][stoi(item_id)] = stof(rating);
      Items[stoi(item_id)][stoi(user_id)] = stof(rating);

    }
  }
  void WriteResults(string testpath, string resultpath) { // düzgün çalışmazsa burayı değiştir
    string testdata;
    ifstream test(testpath);
    ofstream results(resultpath);
    results << "ID,Predicted\n";
    getline(test, testdata);
    while (test.peek() != EOF) {
      getline(test, testdata, ',');
      int id = atoi(testdata.c_str());
      getline(test, testdata, ',');
      int user_id = atoi(testdata.c_str());
      getline(test, testdata);
      int item_id = atoi(testdata.c_str());
      results << id << "," << (UBFC(user_id, item_id) + IBFC(user_id, item_id)) / 2 << "\n";
    }
  }

  float UserSim(int user1, int user2) {
    map<int, float> mapofuser1 = Users[user1];
    map<int, float> mapofuser2 = Users[user2];
    map<int, float>::iterator i1 = mapofuser1.begin();// 1,3,15,34,233
    map<int, float>::iterator i2 = mapofuser2.begin();// 2,3,15,23,27,34
    vector<float> v1, v2;
    while (i1 != mapofuser1.end() && i2 != mapofuser2.end()) {
      if (i1->first == i2->first) {
        v1.push_back(i1->second);
        v2.push_back(i2->second);
        i1++; i2++;
      }
      else if (i1->first < i2->first)i1++;
      else i2++;
    }
    return Cosine(v1, v2);
  }

  float UBFC(int user_id, int item_id) {
    map<int, float> item = Items[item_id];
    map<int, float>::iterator i = item.begin();
    int num = 0;
    float ratings = 0;
    while (i != item.end()) {
      if (UserSim(user_id, i->first) > 0.5) {
        num++;
        ratings += i->second;
      }
      i++;
    }
    return ratings / num;
  }
  float IBFC(int user_id, int item_id) {
    map<int, float> user = Users[user_id];
    map<int, float>::iterator i = user.begin();
    int num = 0;
    float ratings = 0;
    while (i != user.end()) {
      if (ItemSim(item_id, i->first) > 0.5) {
        num++;
        ratings += i->second;
      }
      i++;
    }
    return ratings / num;
  }


float ItemSim(int item1, int item2) {
  map<int, float> mapofitem1 = Items[item1];
  map<int, float> mapofitem2 = Items[item2];
  map<int, float>::iterator i1 = mapofitem1.begin();
  map<int, float>::iterator i2 = mapofitem2.begin();
  vector<float> v1, v2;
  while (i1 != mapofitem1.end() && i2 != mapofitem2.end()) {
    if (i1->first == i2->first) {
      v1.push_back(i1->second);
      v2.push_back(i2->second);
      i1++; i2++;
    }
    else if (i1->first < i2->first) i1++;
    else i2++;
  }
  return Cosine(v1, v2);
}
void ShowUsers() {
  unordered_map<int, map<int, float>>::iterator i = Users.begin();
  vector<user> UsersBest;
  while (i != Users.end()) {
    user u;
    u.userid = i->first;
    u.n = i->second.size();
    UsersBest.push_back(u);
    i++;
  }
  sort(UsersBest.begin(), UsersBest.end(), CompareUsers);
  cout << "Top 10 Users" << endl;
  for (int i = 0; i < 10; i++) {
    cout << "user " << UsersBest[i].userid << " == " << UsersBest[i].n << endl;
  }
}

void ShowItems() {
  unordered_map<int, map<int, float>>::iterator i = Items.begin();
  vector<item> ItemsBest;
  while (i != Items.end()) {
    item j;
    j.itemid = i->first;
    j.n = i->second.size();
    ItemsBest.push_back(j);
    i++;
  }
  sort(ItemsBest.begin(), ItemsBest.end(), CompareItem);
  cout << "Top 10 Items" << endl;
  for (int i = 0; i < 10; i++) {
    cout << "item " << ItemsBest[i].itemid << " == " << ItemsBest[i].n << endl;
  }
}
};


int main() {

  Prediction pr("train.csv", "test.csv", "result.csv");
  pr.ShowItems();
  pr.ShowUsers();
return 0;
}