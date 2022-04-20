#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <bits/stdc++.h>
using namespace std;



class MealPlan{
public:
    struct Food{
        float foodGroup;
        string name;
        float calories;
        float protein;
        float carb;
        float fat;
        float grams;
    };

    float totalCal;
    float totalPro;
    float totalFat;
    float totalCarb;
    int numOfBuckets;
    float maxLoadFact;
    float sz;
    Food** table;
    Food** breakfast;
    int bSize;
    Food** entrees;
    int eSize;
    Food** sides;
    int sSize;
    float scale;

    MealPlan(float cal, float protein, float fat, float carb);
    int HashFunction(string &name);
    void insertFood(float foodGroup, string &name, float cals, float protein, float fat, float carbs, float grams);
    void Resize();
    void sortFoodGroup();
    void heapifyBreakfast(int size, int index);
    void heapifyEntrees(int size, int index);
    void heapifySides(int size, int index);
    void getBreakfast() const;
    void getLunch() const;
    void getDinner();
};

MealPlan::MealPlan(float cal, float protein, float fat, float carb) {
    totalCal = cal;
    totalPro = protein;
    totalCarb = carb;
    totalFat = fat;
    sz = 0;
    numOfBuckets = 100;
    maxLoadFact = 0.8;
    table = new Food*[numOfBuckets];
    for(int i = 0; i < numOfBuckets; i++){
        table[i] = nullptr;
    }
    scale = 0.75;
    float n = 0;
    while(totalPro > n){
        scale += 0.25;
        n += 50;
    }
    bSize = 0;
    sSize = 0;
    eSize = 0;
}
int MealPlan::HashFunction(string &name){
    return (name.size() % numOfBuckets);
}
void MealPlan::insertFood(float foodGroup, string &name, float cals, float protein, float fat, float carbs, float grams) {
    Food* newFood = new Food;
    newFood->foodGroup = foodGroup;
    newFood->name = name;
    newFood->calories = cals;
    newFood->protein = protein;
    newFood->fat = fat;
    newFood->carb = carbs;
    newFood->grams = grams;
    if ((foodGroup == 1 && (name[0] == 'E' || name[0] == 'Y')) || foodGroup == 8 || (foodGroup == 9 && name[0] != 'U')|| foodGroup == 12 || (foodGroup == 18 && name[0] != 'L'))
        bSize++;
    else if (foodGroup == 5 || (foodGroup == 7 && name[0] != 'O' && name[0] != 'L') || foodGroup == 10 || foodGroup == 13 || foodGroup == 15 || foodGroup == 23 || (foodGroup == 6 && name[1] == 'O') || foodGroup == 27 || foodGroup == 22)
        eSize++;
    else if (foodGroup == 11 || foodGroup == 16 || foodGroup == 20)
        sSize++;

    int index = HashFunction(name);
    int prob = 0;
    while(table[((index + prob*prob) % numOfBuckets)] != nullptr){
        prob++;
    }
    int indexFin = ((index + prob*prob) % numOfBuckets);
    if(((index + prob*prob) % numOfBuckets) < 0){
        indexFin = (-1)*((index + prob*prob) % numOfBuckets);
    }
    table[indexFin] = newFood;
    sz++;

    if(sz/numOfBuckets >= maxLoadFact)
        Resize();

}
void MealPlan::Resize() {
    sz = 0;
    Food* temp[numOfBuckets];
    for(int i = 0; i < numOfBuckets; i++){
        if(table[i] != nullptr) {
            temp[i] = table[i];
        }
        else{
            temp[i] = nullptr;
        }
    }

    table = new Food*[numOfBuckets*2];
    for(int i = 0; i < numOfBuckets*2; i++){
        table[i] = nullptr;
    }

    numOfBuckets *= 2;
    for(int i = 0; i < numOfBuckets/2; i++){
        if(temp[i] != nullptr) {
            insertFood(temp[i]->foodGroup, temp[i]->name, temp[i]->calories, temp[i]->protein, temp[i]->fat, temp[i]->carb, temp[i]->grams);
        }
    }
}

void MealPlan::sortFoodGroup() {
    breakfast = new Food*[bSize];
    entrees = new Food*[eSize];
    sides = new Food*[sSize];

    int index1 = 0;
    int index2 = 0;
    int index3 = 0;

    for(int i = 0; i < numOfBuckets; i++){
        if(table[i] == nullptr) {
            continue;
        }
        if((table[i]->foodGroup == 1 && (table[i]->name[0] == 'E' || table[i]->name[0] == 'Y')) || table[i]->foodGroup == 8 || (table[i]->foodGroup == 9 && table[i]->name[0] != 'U')|| table[i]->foodGroup == 12 || (table[i]->foodGroup == 18 && table[i]->name[0] != 'L')){
            breakfast[index1] = table[i];
            index1++;
        }
        else if(table[i]->foodGroup == 5 || (table[i]->foodGroup == 7 && table[i]->name[0] != 'O' && table[i]->name[0] != 'L') || table[i]->foodGroup == 10 || table[i]->foodGroup == 13 || table[i]->foodGroup == 15 || table[i]->foodGroup == 23 || (table[i]->foodGroup == 6 && table[i]->name[1] == 'O') || table[i]->foodGroup == 27 || table[i]->foodGroup == 22){
            entrees[index2] = table[i];
            index2++;
        }
        else if(table[i]->foodGroup == 11 || table[i]->foodGroup == 16 || table[i]->foodGroup == 20){
            sides[index3] = table[i];
            index3++;
        }
    }

    int start = ((bSize)/2) - 1;
    for(int i = start; i >= 0; i--)
        heapifyBreakfast(index1, i);
    start = ((eSize)/2) - 1;
    for(int i = start; i >= 0; i--)
        heapifyEntrees(index2, i);
    start = ((sSize)/2) - 1;
    for(int i = start; i >= 0; i--)
        heapifySides(index3, i);
}

void MealPlan::heapifyBreakfast(int size, int index){
    //max heap, only inserting so heapify up
    int largest = index;
    int l = 2 * index + 1;
    int r = 2 * index + 2;

    if(l < size && breakfast[l]->calories > breakfast[largest]->calories)
        largest = l;
    if(r < size && breakfast[r]->calories > breakfast[largest]->calories)
        largest = r;
    if(largest != index){
        swap(breakfast[index], breakfast[largest]);
        heapifyBreakfast(size, largest);
    }
}
void MealPlan::heapifyEntrees(int size, int index){
    //max heap, only inserting so heapify up
    int largest = index;
    int l = 2 * index + 1;
    int r = 2 * index + 2;

    if(l < size && entrees[l]->calories > entrees[largest]->calories)
        largest = l;
    if(r < size && entrees[r]->calories > entrees[largest]->calories)
        largest = r;
    if(largest != index){
        swap(entrees[index], entrees[largest]);
        heapifyEntrees(size, largest);
    }
}
void MealPlan::heapifySides(int size, int index){
    //max heap, only inserting so heapify up
    int largest = index;
    int l = 2 * index + 1;
    int r = 2 * index + 2;

    if(l < size && sides[l]->calories > sides[largest]->calories)
        largest = l;
    if(r < size && sides[r]->calories > sides[largest]->calories)
        largest = r;
    if(largest != index){
        swap(sides[index], sides[largest]);
        heapifySides(size, largest);
    }
}

void MealPlan::getBreakfast() const {
    float calGoal = totalCal/4;
    float proGoal = totalPro/4;
    float carbGoal = totalCarb/4;
    float count = 0;
    float group = 0;
    int meal = 0;
    bool check = false;
    vector<string> used;
    cout << left << setw(35) << " " << left << setw(35) << "Breakfast:\n------------------------------------------------------------------------------------------" << endl;
    cout << left  << setw(15) << "Amount (g)" << left << setw(25) << "Name"  << left << setw(13) << "Calories" << left << setw(13)
         << "Protein" << left << setw(13) << "Fat" << left << setw(13) << "Carbs\n------------------------------------------------------------------------------------------" << endl;
    for(int i = 0; i < bSize; i++){
        float temp = scale*breakfast[i]->calories - (calGoal/2.0);
        float temp2 = abs(scale*breakfast[i]->protein - (proGoal/2.0));
        if(temp <= 30 && (temp2 <= 15 || check) && breakfast[i]->foodGroup != group ){
            string name = breakfast[i]->name;
            if (find(used.begin(),used.end(), name) != used.end()) continue;
            used.push_back(name);
            cout << left  << setw(15) << scale*breakfast[i]->grams  << left << setw(25) << name << left << setw(13) << scale*breakfast[i]->calories << left << setw(13) << scale*breakfast[i]->protein << left << setw(13) << scale*breakfast[i]->fat << left << setw(13) << scale*breakfast[i]->carb << endl;
            count += scale*breakfast[i]->calories;
            group = breakfast[i]->foodGroup;
            carbGoal -= scale*breakfast[i]->carb;
            check = true;
            if(count >= calGoal)
                break;
        }
    }
}
void MealPlan::getDinner() {
    float calGoal = totalCal/2;
    float proGoal = totalPro/2;
    float carbGoal = totalCarb/2;
    cout << left << setw(35) << " " << left << setw(35) << "Dinner:\n------------------------------------------------------------------------------------------" << endl;
    cout << left  << setw(15) << "Amount (g)" << left << setw(25) << "Name"  << left << setw(13) << "Calories" << left << setw(13)
         << "Protein" << left << setw(13) << "Fat" << left << setw(13) << "Carbs\n------------------------------------------------------------------------------------------" << endl;
    vector<string> used;
    for(int i = 0; i < eSize; i += 2){
        float temp = scale*entrees[i]->calories - (calGoal/2.0);
        float temp2 = abs(scale*entrees[i]->protein - ((2.0/3.0)*proGoal));
        if(temp <= 30 && temp2 <= 10) {
            string name = entrees[i]->name;
            if (find(used.begin(),used.end(), name) != used.end()) continue;
            used.push_back(name);
            cout << left  << setw(15) << scale*entrees[i]->grams << left  << setw(25) << name << left  << setw(13) << scale*entrees[i]->calories << left  << setw(13) << scale*entrees[i]->protein << left  << setw(13) << scale*entrees[i]->fat << left  << setw(13) << scale*entrees[i]->carb << endl;
            calGoal -= scale*entrees[i]->calories;
            proGoal -= scale*entrees[i]->protein;
            break;
        }
    }
    float count = 0;
    float group = 0;
    for(int i = 0; i < sSize; i += 2){
        float temp = scale*sides[i]->calories - (calGoal/2.0);
        float temp2 = scale*sides[i]->protein - proGoal;
        float temp3 = scale*sides[i]->carb - carbGoal;
        if(temp <= 30 && (temp2 <= 10 || temp3 <= 15) && sides[i]->foodGroup != group){ //&& sides[i]->carb <= (temp) && sides[i]->protein <= (temp)
            string name = sides[i]->name;
            if (find(used.begin(),used.end(), name) != used.end()) continue;
            used.push_back(name);
            cout << left  << setw(15) << scale*sides[i]->grams << left  << setw(25) << name << left  << setw(13) << scale*sides[i]->calories << left  << setw(13) << scale*sides[i]->protein << left  << setw(13) << scale*sides[i]->fat << left  << setw(13) << scale*sides[i]->carb << endl;
            count += scale*sides[i]->calories;
            group = sides[i]->foodGroup;
            if(count >= (calGoal - 50))
                break;
        }
    }
}
void MealPlan::getLunch() const {
    float calGoal = totalCal/4;
    float carbGoal = totalCarb/4;
    float proGoal = totalPro/4;
    vector<string> used;
    cout << left << setw(35) << " " << left << setw(35) <<  "Lunch:\n------------------------------------------------------------------------------------------" << endl;
    cout << left  << setw(15) << "Amount (g)" << left << setw(25) << "Name"  << left << setw(13) << "Calories" << left << setw(13)
         << "Protein" << left << setw(13) << "Fat" << left << setw(13) << "Carbs\n------------------------------------------------------------------------------------------" << endl;
    for(int i = 1; i < eSize; i += 2){
        float temp = scale*entrees[i]->calories - (calGoal/2);
        float temp2 = abs(scale*entrees[i]->protein - ((2.0/3)*proGoal));
        if(temp <= 30 && temp2 <= 15) {
            string name = entrees[i]->name;
            if (find(used.begin(),used.end(), name) != used.end()) continue;
            used.push_back(name);
            cout << left  << setw(15) << scale*entrees[i]->grams << left  << setw(25) << name << left  << setw(13) << scale*entrees[i]->calories << left  << setw(13) << scale*entrees[i]->protein << left  << setw(13) << scale*entrees[i]->fat << left  << setw(13) << scale*entrees[i]->carb << endl;
            calGoal -= scale*entrees[i]->calories;
            proGoal -= scale*entrees[i]->protein;
            break;
        }
    }
    float count = 0;
    float group = 0;
    for(int i = 1; i < sSize; i += 2){
        float temp = (scale/1.5)*sides[i]->calories - (calGoal/2);
        float temp2 = (scale/1.5)*sides[i]->protein - proGoal;
        float temp3 = (scale/1.5)*sides[i]->carb - carbGoal;
        if(temp <= 30 && (temp2 <= 10 || temp3 <= 15) && sides[i]->foodGroup != group){ //&& sides[i]->carb <= (temp) && sides[i]->protein <= (temp)
            string name = sides[i]->name;
            if (find(used.begin(),used.end(), name) != used.end()) continue;
            used.push_back(name);
            cout << left  << setw(15) << (scale/1.5)*sides[i]->grams << left  << setw(25) << name << left  << setw(13) << (scale/1.5)*sides[i]->calories << left  << setw(13) <<  (scale/1.5)*sides[i]->protein << left  << setw(13) << (scale/1.5)*sides[i]->fat << left  << setw(13) << (scale/1.5)*sides[i]->carb << endl;
            count += (scale/1.5)*sides[i]->calories;
            group = sides[i]->foodGroup;
            if(count >= (calGoal - 50))
                break;
        }
    }
}

float BMR(float weight, float height, float age, float gender){
    float bmr;
    if(gender == 1)
        bmr = 166.47 + (6.24*weight) + (12.7*height) - (6.75*age);
    else
        bmr =(4.536 * weight) + (15.88 * height) - (5*age);

    return bmr;
}

void readLine(istringstream &stream, MealPlan& mealPlan, float cap){
    float foodGroup;
    string name;
    float protein;
    float cal;
    float fat;
    float carb;
    float grams;
    for(int i = 0; i < 11; i++){
        if(i == 0) {
            string temp;
            getline(stream, temp, '\t');
            if(temp.size() == 4)
                foodGroup = stof(temp.substr(0,1));
            else if((temp[0] == '1' && temp[1] == '6') || (temp[0] == '2' && temp[1] == '0')){
                foodGroup = stof(temp);
                if((foodGroup >= 16006 && foodGroup <= 16051) || (foodGroup >= 16315 && foodGroup <= 16384))
                    foodGroup = stof(temp.substr(0,2));
                else if((foodGroup >= 20040 && foodGroup <= 20058) || (foodGroup >= 20091 && foodGroup <= 20127)) {
                    foodGroup = stof(temp.substr(0, 2));
                }
                else
                    continue;
            }
            else
                foodGroup = stof(temp.substr(0,2));
        }
        else if(i == 3){
            string temp;
            getline(stream, temp, '\t');
            if(foodGroup == 12) {
                string temp1 = temp.substr(temp.find(',') + 2, temp.size() - 1);
                name = temp1.substr(0, temp1.find(','));
                name[0] = toupper(name[0]);

            }
            else
                name = temp.substr(1, temp.find(',') - 1);
            if (name.find('(') != string::npos)
                name = name.substr(0, name.find('(') - 2);
        }
        else if(i == 7){
            string temp;
            getline(stream, temp, '\t');
            cal = stof(temp);
        }
        else if(i == 8){
            string temp;
            getline(stream, temp, '\t');
            protein = stof(temp);
        }
        else if(i == 9){
            string temp;
            getline(stream, temp, '\t');
            fat = stof(temp);
        }
        else if(i == 10){
            string temp;
            getline(stream, temp, '\t');
            carb = stof(temp);
        }
        else{
            string temp;
            getline(stream, temp,'\t');
        }
        grams = 100;
    }
    if(foodGroup == 12 || foodGroup == 19){
        cal /= 2;
        protein /= 2;
        fat /= 2;
        carb /= 2;
        grams /= 2;
    }
    else if(carb > cap){
        cal /= 2;
        protein /= 2;
        fat /= 2;
        carb /= 2;
        grams /= 2;
    }

    mealPlan.insertFood(foodGroup, name, cal, protein, fat, carb, grams);
}
int main() {
    float age;
    float weight;
    float height;
    float gender;
    float goalWeight;
    float cal;
    float protein;
    float carb;
    float fat;
    cout << "*****************************************\n";
    cout << "* Welcome to your Make Your Meal Ideal! *" << endl;
    cout << "*****************************************\n";
    cout << "To get a meal plan, please insert the following information." << endl;
    cout << "Weight:";
    cin >> weight;
    cout << "\nHeight (in inches):";
    cin >> height;
    cout << "\nAge:";
    cin >> age;
    cout << "\nGender (1 for male, 2 for female):";
    cin >> gender;
    cout << "\nGoal Weight:";
    cin >> goalWeight;


    cal = BMR(weight, height, age, gender);
    cout << "\n\nYour resting basal metabolic rate (BMR): " << cal << endl;
    if(weight > goalWeight) {
        cal -= 500;
        if(cal <= (9*protein))
            cal *= 1.1;
        protein = weight;
        carb = weight * 1.5;
        fat = weight * 0.3;
    }
    else if(goalWeight > weight) {
        cal += 500;
        protein = weight;
        if(cal >= (12*protein))
            cal *= 0.90;
        carb = weight * 1.5;
        fat = weight * 0.5;
    }
    else{
        protein = weight;
        carb = weight * 1.5;
        fat = weight * 0.4;
    }


    MealPlan mealPlan(cal, protein, fat, carb);
    cout << "\nBased on your goal, below is your recommended daily macronutrient intake range:" << endl;
    cout << "-------------------------------------------------------------------------------" << endl;
    if(weight - goalWeight > 0)
        cout << "Calories: \t" << cal << " to " << cal + 150 << " cal" << endl;
    else
        cout << "Calories: \t" << cal - 150 << " to " << cal << " cal" << endl;
    cout << "Protein: \t" << protein*(2.0/3) << " to " << protein << " g" << endl;
    cout << "Carbohydrate: \t" << carb*(1.5/3) << " to " << carb*(2.5/3) << " g\n" << endl;


    ifstream file;
    file.open("nndb_flat.txt");

    string line;
    if (file.is_open())
        getline(file, line);

    while (getline(file, line)) {
        istringstream stream(line);
        readLine(stream, mealPlan, carb/8);
    }
    file.close();


    mealPlan.sortFoodGroup();
    mealPlan.getBreakfast();
    cout << endl << endl;
    mealPlan.getLunch();
    cout << endl << endl;
    mealPlan.getDinner();
    cout << endl << endl;
    cout << "*NOTE* this is the nutrition information for little to no physical activity. If you are active, you must consume\nadditional calories each day equivalent to the amount burned while active.\n" << endl;
    cout << "IF MEAL PLAN IS FOLLOWED STRICTLY\n----------------------------------------" << endl;
    cout << "Time Period to Reach " << goalWeight << " Pounds: " << abs(weight-goalWeight) << " weeks" << endl;
    cout << endl << endl;
    cout << "*****************************************\n";
    cout << "*                                       *\n";
    cout << "*               GOOD LUCK!              *\n";
    cout << "*                                       *\n";
    cout << "*****************************************\n";
    cout << endl << endl;
    cout << "Gainesville Gains Gang" << endl;
}
