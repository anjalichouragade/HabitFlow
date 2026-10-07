#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <ctime>
#include <sstream>

using namespace std;

class Habit
{
private:
    int id;
    string name;
    string category;
    vector<string> completedDates;

public:

    // Constructor
    Habit(int i, string n, string c)
    {
        id = i;
        name = n;
        category = c;
    }

    // Default constructor for file loading
    Habit()
    {
        id = 0;
        name = "";
        category = "";
    }

    int getId()
    {
        return id;
    }

    string getName()
    {
        return name;
    }

    string getCategory()
    {
        return category;
    }

    void setName(string n)
    {
        name = n;
    }

    void setCategory(string c)
    {
        category = c;
    }

    // Get today's date
    string getTodayDate()
    {
        time_t now = time(0);
        tm *localTime = localtime(&now);

        int day = localTime->tm_mday;
        int month = localTime->tm_mon + 1;
        int year = localTime->tm_year + 1900;

        stringstream ss;
        ss << year << "-";

        if(month < 10)
            ss << "0";

        ss << month << "-";

        if(day < 10)
            ss << "0";

        ss << day;

        return ss.str();
    }

    // Check if today is completed
    bool isCompletedToday()
    {
        string today = getTodayDate();

        for(int i = 0; i < completedDates.size(); i++)
        {
            if(completedDates[i] == today)
                return true;
        }

        return false;
    }

    // Mark today's habit
    void toggleToday()
    {
        string today = getTodayDate();

        for(int i = 0; i < completedDates.size(); i++)
        {
            if(completedDates[i] == today)
            {
                completedDates.erase(completedDates.begin() + i);
                return;
            }
        }

        completedDates.push_back(today);
    }

    // Display habit
    void display()
    {
        cout << "\n-------------------------------";
        cout << "\nHabit ID   : " << id;
        cout << "\nHabit Name : " << name;
        cout << "\nCategory   : " << category;

        cout << "\nToday      : ";

        if(isCompletedToday())
            cout << "[✓]";
        else
            cout << "[ ]";

        cout << "\nCurrent Streak : " << getCurrentStreak() << " days";
        cout << "\nLongest Streak : " << getLongestStreak() << " days";

        cout << "\n-------------------------------\n";
    }

    // Calculate current streak
    int getCurrentStreak()
    {
        if(completedDates.size() == 0)
            return 0;

        int streak = 0;

        time_t now = time(0);

        for(int dayBack = 0; dayBack < 3650; dayBack++)
        {
            time_t checkTime = now - (dayBack * 24 * 60 * 60);

            tm *checkDate = localtime(&checkTime);

            int day = checkDate->tm_mday;
            int month = checkDate->tm_mon + 1;
            int year = checkDate->tm_year + 1900;

            stringstream ss;
            ss << year << "-";

            if(month < 10)
                ss << "0";

            ss << month << "-";

            if(day < 10)
                ss << "0";

            ss << day;

            string date = ss.str();

            bool found = false;

            for(int i = 0; i < completedDates.size(); i++)
            {
                if(completedDates[i] == date)
                {
                    found = true;
                    break;
                }
            }

            if(found)
                streak++;
            else
                break;
        }

        return streak;
    }

    // Calculate longest streak
    int getLongestStreak()
    {
        if(completedDates.size() == 0)
            return 0;

        int longest = 0;

        for(int start = 0; start < completedDates.size(); start++)
        {
            int streak = 1;

            for(int next = start + 1; next < completedDates.size(); next++)
            {
                int year1, month1, day1;
                int year2, month2, day2;

                sscanf(completedDates[start].c_str(),
                       "%d-%d-%d",
                       &year1, &month1, &day1);

                sscanf(completedDates[next].c_str(),
                       "%d-%d-%d",
                       &year2, &month2, &day2);

                tm date1 = {};
                date1.tm_year = year1 - 1900;
                date1.tm_mon = month1 - 1;
                date1.tm_mday = day1;

                tm date2 = {};
                date2.tm_year = year2 - 1900;
                date2.tm_mon = month2 - 1;
                date2.tm_mday = day2;

                time_t time1 = mktime(&date1);
                time_t time2 = mktime(&date2);

                double difference = difftime(time2, time1);

                if(difference == 86400)
                {
                    streak++;
                }
                else
                {
                    break;
                }
            }

            if(streak > longest)
                longest = streak;
        }

        return longest;
    }

    // Save habit to file
    void save(ofstream &file)
    {
        file << id << "|"
             << name << "|"
             << category << "|";

        for(int i = 0; i < completedDates.size(); i++)
        {
            file << completedDates[i];

            if(i < completedDates.size() - 1)
                file << ",";
        }

        file << "\n";
    }

    // Load habit from file
    void loadData(string data)
    {
        vector<string> parts;
        string part;
        stringstream ss(data);

        while(getline(ss, part, '|'))
        {
            parts.push_back(part);
        }

        if(parts.size() >= 3)
        {
            id = stoi(parts[0]);
            name = parts[1];
            category = parts[2];

            completedDates.clear();

            if(parts.size() >= 4)
            {
                stringstream dates(parts[3]);
                string date;

                while(getline(dates, date, ','))
                {
                    if(date != "")
                        completedDates.push_back(date);
                }
            }
        }
    }
};


// Save all habits
void saveHabits(vector<Habit> &habits)
{
    ofstream file("habits.txt");

    for(int i = 0; i < habits.size(); i++)
    {
        habits[i].save(file);
    }

    file.close();
}


// Load all habits
void loadHabits(vector<Habit> &habits)
{
    ifstream file("habits.txt");

    string line;

    while(getline(file, line))
    {
        if(line != "")
        {
            Habit h;
            h.loadData(line);
            habits.push_back(h);
        }
    }

    file.close();
}


int main()
{
    vector<Habit> habits;

    loadHabits(habits);

    int choice;

    cout << "====================================\n";
    cout << "          🌸 HABITFLOW 🌸\n";
    cout << "       Daily Habit Tracker\n";
    cout << "====================================\n";

    do
    {
        cout << "\n1. Add Habit";
        cout << "\n2. View Habits";
        cout << "\n3. Mark Today's Habit";
        cout << "\n4. Search Habit";
        cout << "\n5. Edit Habit";
        cout << "\n6. Delete Habit";
        cout << "\n7. Show Progress";
        cout << "\n8. Show Streaks";
        cout << "\n9. Exit";

        cout << "\n\nEnter your choice: ";
        cin >> choice;

        switch(choice)
        {
            // ADD HABIT
            case 1:
            {
                int id;
                string name;
                string category;

                cout << "\nEnter Habit ID: ";
                cin >> id;

                cin.ignore();

                cout << "Enter Habit Name: ";
                getline(cin, name);

                cout << "Enter Category: ";
                getline(cin, category);

                Habit h(id, name, category);

                habits.push_back(h);

                saveHabits(habits);

                cout << "\nHabit added successfully! 🌸\n";

                break;
            }

            // VIEW HABITS
            case 2:
            {
                cout << "\n========== YOUR HABITS ==========\n";

                if(habits.size() == 0)
                {
                    cout << "\nNo habits available.\n";
                }
                else
                {
                    for(int i = 0; i < habits.size(); i++)
                    {
                        habits[i].display();
                    }
                }

                break;
            }

            // MARK TODAY'S HABIT
            case 3:
            {
                if(habits.size() == 0)
                {
                    cout << "\nNo habits available.\n";
                    break;
                }

                cout << "\n========== TODAY'S HABITS ==========\n";

                for(int i = 0; i < habits.size(); i++)
                {
                    cout << "\n"
                         << i + 1 << ". "
                         << habits[i].getName()
                         << "  ";

                    if(habits[i].isCompletedToday())
                        cout << "[✓]";
                    else
                        cout << "[ ]";
                }

                int number;

                cout << "\n\nEnter habit number to toggle: ";
                cin >> number;

                if(number >= 1 && number <= habits.size())
                {
                    habits[number - 1].toggleToday();

                    saveHabits(habits);

                    cout << "\nToday's habit updated! ✓\n";
                }
                else
                {
                    cout << "\nInvalid habit number!\n";
                }

                break;
            }

            // SEARCH HABIT
            case 4:
            {
                int id;
                bool found = false;

                cout << "\nEnter Habit ID to search: ";
                cin >> id;

                for(int i = 0; i < habits.size(); i++)
                {
                    if(habits[i].getId() == id)
                    {
                        habits[i].display();

                        found = true;
                        break;
                    }
                }

                if(!found)
                    cout << "\nHabit not found!\n";

                break;
            }

            // EDIT HABIT
            case 5:
            {
                int id;
                bool found = false;

                cout << "\nEnter Habit ID to edit: ";
                cin >> id;

                for(int i = 0; i < habits.size(); i++)
                {
                    if(habits[i].getId() == id)
                    {
                        string newName;
                        string newCategory;

                        cin.ignore();

                        cout << "\nEnter New Habit Name: ";
                        getline(cin, newName);

                        cout << "Enter New Category: ";
                        getline(cin, newCategory);

                        habits[i].setName(newName);
                        habits[i].setCategory(newCategory);

                        saveHabits(habits);

                        found = true;

                        cout << "\nHabit updated successfully!\n";

                        break;
                    }
                }

                if(!found)
                    cout << "\nHabit not found!\n";

                break;
            }

            // DELETE HABIT
            case 6:
            {
                int id;
                bool found = false;

                cout << "\nEnter Habit ID to delete: ";
                cin >> id;

                for(int i = 0; i < habits.size(); i++)
                {
                    if(habits[i].getId() == id)
                    {
                        habits.erase(habits.begin() + i);

                        saveHabits(habits);

                        found = true;

                        cout << "\nHabit deleted successfully!\n";

                        break;
                    }
                }

                if(!found)
                    cout << "\nHabit not found!\n";

                break;
            }

            // SHOW PROGRESS
            case 7:
            {
                int total = habits.size();
                int completedToday = 0;

                for(int i = 0; i < habits.size(); i++)
                {
                    if(habits[i].isCompletedToday())
                        completedToday++;
                }

                cout << "\n========== TODAY'S PROGRESS ==========\n";

                cout << "Total Habits     : " << total;
                cout << "\nCompleted Today  : " << completedToday;
                cout << "\nPending Today    : "
                     << total - completedToday;

                if(total > 0)
                {
                    float percentage =
                        (completedToday * 100.0) / total;

                    cout << "\nCompletion Rate  : "
                         << percentage << "%";
                }
                else
                {
                    cout << "\nCompletion Rate  : 0%";
                }

                cout << "\n=======================================\n";

                break;
            }

            // SHOW STREAKS
            case 8:
            {
                cout << "\n========== HABIT STREAKS ==========\n";

                if(habits.size() == 0)
                {
                    cout << "\nNo habits available.\n";
                }
                else
                {
                    for(int i = 0; i < habits.size(); i++)
                    {
                        cout << "\n"
                             << habits[i].getName();

                        cout << "\nCurrent Streak : "
                             << habits[i].getCurrentStreak()
                             << " days";

                        cout << "\nLongest Streak : "
                             << habits[i].getLongestStreak()
                             << " days";

                        cout << "\n-------------------------";
                    }
                }

                cout << "\n";

                break;
            }

            // EXIT
            case 9:
            {
                saveHabits(habits);

                cout << "\nThank you for using HabitFlow! 🌸\n";

                break;
            }

            default:
            {
                cout << "\nInvalid choice! Please try again.\n";
            }
        }

    } while(choice != 9);

    return 0;
}