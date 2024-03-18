#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <cmath>

using namespace std;

const int INF = 1000 * 1000 * 1000;

class MyDate {
    //const char* datetimeString = "2023-06-17 12:36";
    static const int seconds_per_day = 60*60*24;
    static const int timezone_h = 2;
    static constexpr const string format = "%Y-%m-%d %H:%M";
    tm date;

public:
    MyDate() = default;

    MyDate(const string& dateString) {
        this->setDate(dateString);
    }

    MyDate(const tm& cur_time) {
        this->date = cur_time;
    }

    MyDate(const MyDate& dt) {
        this->date = dt.date;
    }

    int getYear() {
        return this->date.tm_year;
    }

    int getMonth() {
        return this->date.tm_mon;
    }

    int getDay() {
        return this->date.tm_mday;
    }

    int getHour() {
        return this->date.tm_hour;
    }

    int getMinute() {
        return this->date.tm_min;
    }

    std::string getDate() {
        char buffer[80]; // Buffer to hold the formatted string
        strftime(buffer, 80, format.c_str(), &this->date);
        return std::string(buffer); // Convert the buffer to a string and return
    }

    void setDate(const std::string& dateString) {
        std::tm timeStruct = {}; // Initialize to zero
        std::istringstream ss(dateString);
        ss >> get_time(&timeStruct, format.c_str()); // Parse the string using get_time
        if (ss.fail()) {
            std::cerr << "Error: Failed to parse date string." << std::endl;
            // Handle parsing failure, perhaps throw an exception or return an invalid tm structure
        }
        this->date = timeStruct; // Return the parsed tm structure
    }

    void set_time(const int h, const int m) {
        this->date.tm_hour = h;
        this->date.tm_min = m;
    }

    double date_diff(MyDate& dt) {
        std::time_t time1 = std::mktime(&this->date);
        std::time_t time2 = std::mktime(&dt.date);
        double date_difference = std::difftime(time1, time2) / seconds_per_day;
        //cout << MyDate(this->date).getDate() << endl << dt.getDate()<<endl;
        return date_difference;
    }

    void add_hours(const int& h) {
        this->date.tm_hour += h + timezone_h;
        time_t new_time = mktime(&this->date);
        this->date = *gmtime(&new_time);
    }

    friend ostream& operator<<(ostream& out, MyDate&);
};

class Flight {
private:
    MyDate departure_date;
    MyDate arrival_date;
    string departure_city;
    string arrival_city;

    void setDepartureCity(const string &departure) {
        Flight::departure_city = departure;
    }

    void setArrivalCity(const string &arrival) {
        Flight::arrival_city = arrival;
    }

public:
    Flight() = default;
    Flight(const MyDate& dep_date, const MyDate& arrival_date, const string& departure, const string& arrival) {
        this->departure_date = dep_date;
        this->arrival_date = arrival_date;
        this->departure_city = departure;
        this->arrival_city = arrival;
    }

    Flight(const Flight& flight) {
        this->departure_date = flight.departure_date;
        this->arrival_date = flight.arrival_date;
        this->departure_city = flight.departure_city;
        this->arrival_city = flight.arrival_city;
    }

    double get_duration() {
        return round(this->arrival_date.date_diff(this->departure_date) * 60 * 24);
    }

    Flight& operator=(const Flight& f) = default;

    bool operator==(Flight& f) {
        bool flag = this->departure_date.getDate() == f.departure_date.getDate() &&
                this->departure_city == f.departure_city && this->arrival_city == f.arrival_city;
        return flag;
    }

    string getDepartureDate() {
        return this->departure_date.getDate();
    }

    string getArrivalDate() {
        return this->arrival_date.getDate();
    }

    const string &getDepartureCity() {
        return departure_city;
    }

    const string &getArrivalCity() {
        return arrival_city;
    }

    friend ostream& operator<<(ostream& out, Flight&);
    friend ifstream& operator>>(ifstream& in, Flight&);
    friend class Admin;

    ~Flight() = default;
};

class Schedule {
private:
    map<string, vector<Flight>> flights;

    void planFlight(const Flight& flight, MyDate dt) {
        string date = dt.getDate();
        if(this->flights.find(date)==this->flights.end()) {
            vector<Flight> temp_flight;
            temp_flight.push_back(flight);
            this->flights[date] = temp_flight;
        } else {
            this->flights[date].push_back(flight);
        }
    }

    void cancelFlight(Flight& flight) {
        string date = flight.getDepartureDate();
        if(this->flights.find(date)==this->flights.end()) {
            cout << "No such flight" << endl;
            return;
        }
        for(int i=0; i<this->flights[date].size(); i++) {
            if(this->flights[date][i]==flight) {
                this->flights[date].erase(next(this->flights[date].begin(), i));
                cout << "Flight cancelled successfully!" << endl;
                return;
            }
        }
        cout << "No such flight" << endl;
    }

public:
    Schedule() = default;

    void displayFlightsAtTime(MyDate dt) {
        string date = dt.getDate();
        if(this->flights.find(date)==this->flights.end()) {
            cout << "No flights at this time planned!";
        } else {
            cout << "FLights at " << date << endl;
            for(Flight cur_flight: this->flights[date]) {
                cout << cur_flight << endl;
            }
        }
    }

    void displayUpcomingFlights(MyDate& dt) {
        map<string, vector<Flight>>::iterator it;
        bool exist_next = false;
        for (it = this->flights.begin(); it != this->flights.end(); it++)
        {
            MyDate cur_dt(it->first);
            if(cur_dt.date_diff(dt)>0) {
                if(!exist_next) {
                    exist_next = true;
                    cout << "Upcoming flights:\n";
                }
                for(Flight f: it->second) {
                    cout << f << endl;
                }
            }
        }
        if(!exist_next) {
            cout << "No upcoming flights!" << endl;
        }
    }

    void displayPreviousFlights(MyDate& dt) {
        map<string, vector<Flight>>::iterator it;
        bool exist_prev = false;
        for (it = this->flights.begin(); it != this->flights.end(); it++)
        {
            MyDate cur_dt(it->first);
            if(cur_dt.date_diff(dt)<0) {
                if(!exist_prev) {
                    cout << "Previous flights:\n";
                    exist_prev = true;
                }
                for(Flight f: it->second) {
                    cout << f << endl;
                }
            }
        }
        if(!exist_prev) {
            cout << "No previous flights" << endl;
        }
    }

    friend ostream& operator<<(ostream& out, Schedule&);
    friend class Admin;
};

class Admin {
    Schedule schedule;

public:
    Schedule getSchedule() {
        return this->schedule;
    }

    void planFlight(const Flight& flight, MyDate dt) {
        string date = dt.getDate();
        this->schedule.planFlight(flight, date);
    }

    void cancelFlight(Flight& flight) {
        this->schedule.cancelFlight(flight);
    }

    // friend ostream& operator<<(ostream& out, Admin&);
};

ostream& operator<<(ostream& out, Flight& flight) {
    out << flight.departure_date.getDate() << " - " << flight.arrival_date.getDate() << endl;
    out << flight.departure_city << " -> " << flight.arrival_city << endl;
    return out;
}

ostream& operator<<(ostream& out, MyDate& dt) {
    out << dt.getDate() << endl;
    return out;
}

ostream& operator<<(ostream& out, Schedule& sch) {
    map<string, vector<Flight>>::iterator it;
    for (it = sch.flights.begin(); it != sch.flights.end(); it++) {
        for(Flight f: it->second) {
            out << f;
        }
    }
    return out;
}


ifstream& operator >> (ifstream& in, Flight &flight)
{
    string str_date;
    getline(in, str_date);
    flight.departure_date = MyDate(str_date.substr(0, 16));
    flight.arrival_date = MyDate(str_date.substr(19));
    string cities;
    getline(in, cities);
    int idx = (int) cities.find('>');
    flight.departure_city = cities.substr(0, idx-2);
    flight.arrival_city = cities.substr(idx+2);;
    in.ignore(numeric_limits<streamsize>::max(), '\n');
    return in;
}


int main() {

//    std::time_t t = std::time(0);
//    std::tm* now = std::localtime(&t);
//    MyDate dt(*now);
//    MyDate dt11(dt);
//    dt11.add_hours(24*11);
//    dt11.set_time(20, 30);

    MyDate dt("2024-03-29 20:30");
    ifstream fin("tastatura.txt");
    int num_flights;
    fin >> num_flights;
    fin.ignore(numeric_limits<streamsize>::max(), '\n');
    Admin adm;
    for(int i=0; i<num_flights; i++) {
        Flight f;
        fin >> f;
        adm.planFlight(f, f.getDepartureDate());
    }

    Flight to_be_cancelled(dt, dt, "Lisbon", "Madrid");
    adm.getSchedule().displayFlightsAtTime(dt);
    adm.cancelFlight(to_be_cancelled);
    adm.getSchedule().displayFlightsAtTime(dt);

    //adm.getSchedule().displayPreviousFlights(dt);
    //adm.getSchedule().displayUpcomingFlights(dt);
    return 0;
}