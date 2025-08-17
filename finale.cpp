#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <tuple>
#include <random>
#include <iterator>
#include <chrono>
using namespace std;

struct CalendarTeam {
    string name;
    string country;
    string t1 = ""; // home match opponent
    string t2 = ""; // away match opponent
    string t3 = ""; // additional match (if any)
    string t4 = ""; // additional match (if any)
    string t5 = ""; // additional match (if any)
    string t6 = ""; // additional match (if any)
    string t7 = ""; // additional match (if any)
    string t8 = ""; // additional match (if any)

    void print1() const {
        cout << "Team: " << name
             << " | Home vs: " << t1
             << " | Away vs: " << t2
             << " | Home vs: " << t3
             << " | Away vs: " << t4 
             << " | Home vs: " << t5
             << " | Away vs: " << t6
             << " | Home vs: " << t7
             << " | Away vs: " << t8 << endl;
    }

    void print2() const {
        cout << "Team: " << name
        << " | Home vs: " << t3
             << " | Away vs: " << t4
             << " | Home vs: " << t1
             << " | Away vs: " << t2
             << " | Home vs: " << t7
             << " | Away vs: " << t8
             << " | Home vs: " << t5
             << " | Away vs: " << t6 << endl;

    }

    void print3() const {
        cout << "Team: " << name
        << " | Home vs: " << t5
             << " | Away vs: " << t6
             << " | Home vs: " << t7
             << " | Away vs: " << t8
             << " | Home vs: " << t1
             << " | Away vs: " << t2
             << " | Home vs: " << t3
             << " | Away vs: " << t4 << endl;

    }

    void print4() const {
        cout << "Team: " << name
        << " | Home vs: " << t7
             << " | Away vs: " << t8
             << " | Home vs: " << t5
             << " | Away vs: " << t6
             << " | Home vs: " << t3
             << " | Away vs: " << t4
             << " | Home vs: " << t1
             << " | Away vs: " << t2<< endl;

    }

    int matchesAssigned() const {
        int c = 0;
        if (!t1.empty()) ++c;
        if (!t2.empty()) ++c;
        if (!t3.empty()) ++c;
        if (!t4.empty()) ++c;
        return c;
    }

    void reset() {
       t3="" ;
        t4 = "";
        t5 = "";
        t6 = "";
        t7 = "";
        t8 = "";
    }


    const string& getCountry() const {
        return country;
    }

    const string& getName() const {
        return name;
    }
};

// Checks if a team already has 2 matches against teams from specified country
bool is2(const CalendarTeam& team, 
         const vector<CalendarTeam>& allTeams,
         const string& country) {
    vector<string> opponents = {team.t1, team.t2, team.t3, team.t4, team.t5, team.t6, team.t7, team.t8};
    int count = 0;
    for (const auto& opponent_name : opponents) {
        if (opponent_name.empty()) continue;
        auto team_it = find_if(allTeams.begin(), allTeams.end(),
            [&](const CalendarTeam& t) { 
                return t.getCountry() == country && t.getName() == opponent_name; 
            });
        if (team_it != allTeams.end()) {
            count++;
            if (count >= 2) return true;
        }
    }
    return false;
}

vector<int> findPossibleOpponentsSAMEPOT(const CalendarTeam& team,
                                         const vector<CalendarTeam>& teams,
                                         const vector<CalendarTeam>& allTeams,
                                         const string& slot,
                                         bool preferFewestMatches = true) {
    vector<int> indices;
    for (int i = 0; i < teams.size(); ++i) {
        const auto& opp = teams[i];
        if (opp.name == team.name) continue;
        if (opp.country == team.country) continue;
        if ((team.t1 == opp.name || team.t2 == opp.name) ||
            (opp.t1 == team.name || opp.t2 == team.name)) continue;
        if (is2(team, allTeams, opp.country)) continue;
        if (is2(opp, allTeams, team.country)) continue;
        if (slot == "t1" && opp.t2 == "") indices.push_back(i);
        else if (slot == "t2" && opp.t1 == "") indices.push_back(i);
    }
    vector<tuple<int, int, int>> sort_vec;
    for (int idx : indices) {
        int degree = 0;
        for (int j = 0; j < teams.size(); ++j) {
            if (j == idx) continue;
            if (teams[j].country == teams[idx].country) continue;
            if ((teams[idx].t1 == teams[j].name || teams[idx].t2 == teams[j].name) ||
                (teams[j].t1 == teams[idx].name || teams[j].t2 == teams[idx].name)) continue;
            if (slot == "t1" && teams[j].t2 == "") ++degree;
            else if (slot == "t2" && teams[j].t1 == "") ++degree;
        }
        int matches = teams[idx].matchesAssigned();
        sort_vec.emplace_back(matches, degree, idx);
    }
    sort(sort_vec.begin(), sort_vec.end());
    vector<int> result;
    for (auto& tup : sort_vec) result.push_back(get<2>(tup));
    return result;
}

bool lookahead(const vector<CalendarTeam>& pot3, const vector<CalendarTeam>& pot4, const vector<CalendarTeam>& allTeams) {
    for (const auto& team : pot3) {
        int cnt3 = 0, cnt4 = 0;
        if (team.t3 == "") {
            for (const auto& opp : pot4) {
                if (opp.t4 == "" && team.name != opp.name && team.country != opp.country &&
                    team.t4 != opp.name && opp.t3 != team.name && opp.t4 != team.name &&
                    !is2(team, allTeams, opp.country))
                    cnt3++;
            }
            if (cnt3 == 0) return false;
        }
        if (team.t4 == "") {
            for (const auto& opp : pot4) {
                if (opp.t3 == "" && team.name != opp.name && team.country != opp.country &&
                    team.t3 != opp.name && opp.t3 != team.name && opp.t4 != team.name && !is2(team, allTeams, opp.country))
                    cnt4++;
            }
            if (cnt4 == 0) return false;
        }
    }
    for (const auto& team : pot4) {
        int cnt3 = 0, cnt4 = 0;
        if (team.t3 == "") {
            for (const auto& opp : pot3) {
                if (opp.t4 == "" && team.name != opp.name && team.country != opp.country &&
                    team.t4 != opp.name && opp.t3 != team.name && opp.t4 != team.name && !is2(team, allTeams, opp.country)) 
                    cnt3++;
            }
            if (cnt3 == 0) return false;
        }
        if (team.t4 == "") {
            for (const auto& opp : pot3) {
                if (opp.t3 == "" && team.name != opp.name && team.country != opp.country &&
                    team.t3 != opp.name && opp.t3 != team.name && opp.t4 != team.name && !is2(team, allTeams, opp.country))
                    cnt4++;
            }
            if (cnt4 == 0) return false;
        }
    }
    return true;
}

bool lookaheadv2(const vector<CalendarTeam>& firstpot, const vector<CalendarTeam>& secondpot, const vector<CalendarTeam>& allTeams) {
    for (const auto& team : firstpot) {
        int cnt5 = 0, cnt6 = 0;
        if (team.t5 == "") {
            for (const auto& opp : secondpot) {
                if (opp.t6 == "" && team.name != opp.name && team.country != opp.country &&
                    team.t6 != opp.name && opp.t5 != team.name && opp.t6 != team.name &&
                    !is2(team, allTeams, opp.country))
                    cnt5++;
            }
            if (cnt5 == 0) return false;
        }
        if (team.t6 == "") {
            for (const auto& opp : secondpot) {
                if (opp.t5 == "" && team.name != opp.name && team.country != opp.country &&
                    team.t5 != opp.name && opp.t5 != team.name && opp.t6 != team.name && !is2(team, allTeams, opp.country))
                    cnt6++;
            }
            if (cnt6 == 0) return false;
        }
    }
    for (const auto& team : secondpot) {
        int cnt5 = 0, cnt6 = 0;
        if (team.t5 == "") {
            for (const auto& opp : firstpot) {
                if (opp.t6 == "" && team.name != opp.name && team.country != opp.country &&
                    team.t6 != opp.name && opp.t5 != team.name && opp.t6 != team.name && !is2(team, allTeams, opp.country)) 
                    cnt5++;
            }
            if (cnt5 == 0) return false;
        }
        if (team.t6 == "") {
            for (const auto& opp : firstpot) {
                if (opp.t5 == "" && team.name != opp.name && team.country != opp.country &&
                    team.t5 != opp.name && opp.t5 != team.name && opp.t6 != team.name && !is2(team, allTeams, opp.country))
                    cnt6++;
            }
            if (cnt6 == 0) return false;
        }
    }
    return true;
}

bool lookaheadv3(const vector<CalendarTeam>& firstpot, const vector<CalendarTeam>& secondpot, const vector<CalendarTeam>& allTeams) {
    for (const auto& team : firstpot) {
        int cnt7 = 0, cnt8 = 0;
        if (team.t7 == "") {
            for (const auto& opp : secondpot) {
                if (opp.t8 == "" && team.name != opp.name && team.country != opp.country &&
                    team.t8 != opp.name && opp.t7 != team.name && opp.t8 != team.name &&
                    !is2(team, allTeams, opp.country))
                    cnt7++;
            }
            if (cnt7 == 0) return false;
        }
        if (team.t8 == "") {
            for (const auto& opp : secondpot) {
                if (opp.t7 == "" && team.name != opp.name && team.country != opp.country &&
                    team.t7 != opp.name && opp.t7 != team.name && opp.t8 != team.name && !is2(team, allTeams, opp.country))
                    cnt8++;
            }
            if (cnt8 == 0) return false;
        }
    }
    for (const auto& team : secondpot) {
        int cnt7 = 0, cnt8 = 0;
        if (team.t7 == "") {
            for (const auto& opp : firstpot) {
                if (opp.t8 == "" && team.name != opp.name && team.country != opp.country &&
                    team.t8 != opp.name && opp.t7 != team.name && opp.t8 != team.name && !is2(team, allTeams, opp.country)) 
                    cnt7++;
            }
            if (cnt7 == 0) return false;
        }
        if (team.t8 == "") {
            for (const auto& opp : firstpot) {
                if (opp.t7 == "" && team.name != opp.name && team.country != opp.country &&
                    team.t7 != opp.name && opp.t7 != team.name && opp.t8 != team.name && !is2(team, allTeams, opp.country))
                    cnt8++;
            }
            if (cnt8 == 0) return false;
        }
    }
    return true;
}

bool assignMatches(vector<CalendarTeam>& pot3, vector<CalendarTeam>& pot4, const vector<CalendarTeam>& allTeams, int idx1 = 0, int idx2 = 0) {
    for (; idx1 < pot3.size(); ++idx1) {
        if (pot3[idx1].t3 == "") {
            vector<int> candidates;
            for (int i = 0; i < pot4.size(); ++i) {
                if (pot4[i].t4 == "" &&
                    pot3[idx1].name != pot4[i].name &&
                    pot3[idx1].country != pot4[i].country &&
                    pot3[idx1].t4 != pot4[i].name &&
                    pot4[i].t3 != pot3[idx1].name &&
                    pot4[i].t4 != pot3[idx1].name &&
                    !is2(pot3[idx1], allTeams, pot4[i].country) &&
                    !is2(pot4[i], allTeams, pot3[idx1].country))
                    candidates.push_back(i);
            }
            random_device rd; mt19937 g(rd());
            shuffle(candidates.begin(), candidates.end(), g);
            for (int j : candidates) {
                pot3[idx1].t3 = pot4[j].name;
                pot4[j].t4 = pot3[idx1].name;
                if (lookahead(pot3, pot4, allTeams)) {
                    if (assignMatches(pot3, pot4, allTeams, idx1, idx2))
                        return true;
                }
                pot3[idx1].t3 = "";
                pot4[j].t4 = "";
            }
            return false;
        }
        if (pot3[idx1].t4 == "") {
            vector<int> candidates;
            for (int i = 0; i < pot4.size(); ++i) {
                if (pot4[i].t3 == "" &&
                    pot3[idx1].name != pot4[i].name &&
                    pot3[idx1].country != pot4[i].country &&
                    pot3[idx1].t3 != pot4[i].name &&
                    pot4[i].t3 != pot3[idx1].name &&
                    pot4[i].t4 != pot3[idx1].name &&
                    !is2(pot3[idx1], allTeams, pot4[i].country) &&
                    !is2(pot4[i], allTeams, pot3[idx1].country)
                ) candidates.push_back(i);
            }
            random_device rd; mt19937 g(rd());
            shuffle(candidates.begin(), candidates.end(), g);
            for (int j : candidates) {
                pot3[idx1].t4 = pot4[j].name;
                pot4[j].t3 = pot3[idx1].name;
                if (lookahead(pot3, pot4, allTeams)) {
                    if (assignMatches(pot3, pot4, allTeams, idx1, idx2))
                        return true;
                }
                pot3[idx1].t4 = "";
                pot4[j].t3 = "";
            }
            return false;
        }
    }
    for (; idx2 < pot4.size(); ++idx2) {
        if (pot4[idx2].t3 == "") {
            vector<int> candidates;
            for (int i = 0; i < pot3.size(); ++i) {
                if (pot3[i].t4 == "" &&
                    pot4[idx2].name != pot3[i].name &&
                    pot4[idx2].country != pot3[i].country &&
                    pot4[idx2].t4 != pot3[i].name &&
                    pot3[i].t3 != pot4[idx2].name &&
                    pot3[i].t4 != pot4[idx2].name &&
                    !is2(pot4[idx2], allTeams, pot3[i].country) &&
                    !is2(pot3[i], allTeams, pot4[idx2].country))
                    candidates.push_back(i);
            }
            random_device rd; mt19937 g(rd());
            shuffle(candidates.begin(), candidates.end(), g);
            for (int j : candidates) {
                pot4[idx2].t3 = pot3[j].name;
                pot3[j].t4 = pot4[idx2].name;
                if (lookahead(pot3, pot4, allTeams)) {
                    if (assignMatches(pot3, pot4, allTeams, idx1, idx2))
                        return true;
                }
                pot4[idx2].t3 = "";
                pot3[j].t4 = "";
            }
            return false;
        }
        if (pot4[idx2].t4 == "") {
            vector<int> candidates;
            for (int i = 0; i < pot3.size(); ++i) {
                if (pot3[i].t3 == "" &&
                    pot4[idx2].name != pot3[i].name &&
                    pot4[idx2].country != pot3[i].country &&
                    pot4[idx2].t3 != pot3[i].name &&
                    pot3[i].t3 != pot4[idx2].name &&
                    pot3[i].t4 != pot4[idx2].name &&
                    !is2(pot4[idx2], allTeams, pot3[i].country) &&
                    !is2(pot3[i], allTeams, pot4[idx2].country))
                    candidates.push_back(i);
            }
            random_device rd; mt19937 g(rd());
            shuffle(candidates.begin(), candidates.end(), g);
            for (int j : candidates) {
                pot3[idx2].t4 = pot4[j].name;
                pot4[j].t3 = pot3[idx2].name;
                if (lookahead(pot3, pot4, allTeams)) {
                    if (assignMatches(pot3, pot4, allTeams, idx1, idx2))
                        return true;
                }
                pot4[idx2].t4 = "";
                pot3[j].t3 = "";
            }
            return false;
        }
    }
    return true;
}

bool assignMatchesv2(vector<CalendarTeam>& pot1, vector<CalendarTeam>& pot2, const vector<CalendarTeam>& allTeams, int idx1 = 0, int idx2 = 0) {
    for (; idx1 < pot1.size(); ++idx1) {
        if (pot1[idx1].t5 == "") {
            vector<int> candidates;
            for (int i = 0; i < pot2.size(); ++i) {
                if (pot2[i].t6 == "" &&
                    pot1[idx1].name != pot2[i].name &&
                    pot1[idx1].country != pot2[i].country &&
                    pot1[idx1].t6 != pot2[i].name &&
                    pot2[i].t5 != pot1[idx1].name &&
                    pot2[i].t6 != pot1[idx1].name &&
                    !is2(pot1[idx1], allTeams, pot2[i].country) &&
                    !is2(pot2[i], allTeams, pot1[idx1].country)) 
                    candidates.push_back(i);
            }
            random_device rd; mt19937 g(rd());
            shuffle(candidates.begin(), candidates.end(), g);
            for (int j : candidates) {
                pot1[idx1].t5 = pot2[j].name;
                pot2[j].t6 = pot1[idx1].name;
                if (lookaheadv2(pot1, pot2, allTeams)) {
                    if (assignMatchesv2(pot1, pot2, allTeams, idx1, idx2))
                        return true;
                }
                pot1[idx1].t5 = "";
                pot2[j].t6 = "";
            }
            return false;
        }
        if (pot1[idx1].t6 == "") {
            vector<int> candidates;
            for (int i = 0; i < pot2.size(); ++i) {
                if (pot2[i].t5 == "" &&
                    pot1[idx1].name != pot2[i].name &&
                    pot1[idx1].country != pot2[i].country &&
                    pot1[idx1].t5 != pot2[i].name &&
                    pot2[i].t5 != pot1[idx1].name &&
                    pot2[i].t6 != pot1[idx1].name &&
                    !is2(pot1[idx1], allTeams, pot2[i].country) &&
                    !is2(pot2[i], allTeams, pot1[idx1].country))
                    candidates.push_back(i);
            }
            random_device rd; mt19937 g(rd());
            shuffle(candidates.begin(), candidates.end(), g);
            for (int j : candidates) {
                pot1[idx1].t6 = pot2[j].name;
                pot2[j].t5 = pot1[idx1].name;
                if (lookaheadv2(pot1, pot2, allTeams)) {
                    if (assignMatchesv2(pot1, pot2, allTeams, idx1, idx2))
                        return true;
                }
                pot1[idx1].t6 = "";
                pot2[j].t5 = "";
            }
            return false;
        }
    }
    for (; idx2 < pot2.size(); ++idx2) {
        if (pot2[idx2].t5 == "") {
            vector<int> candidates;
            for (int i = 0; i < pot1.size(); ++i) {
                if (pot1[i].t6 == "" &&
                    pot2[idx2].name != pot1[i].name &&
                    pot2[idx2].country != pot1[i].country &&
                    pot2[idx2].t6 != pot1[i].name &&
                    pot1[i].t5 != pot2[idx2].name &&
                    pot1[i].t6 != pot2[idx2].name &&
                    !is2(pot2[idx2], allTeams, pot1[i].country) &&
                    !is2(pot1[i], allTeams, pot2[idx2].country))
                    candidates.push_back(i);
            }
            random_device rd; mt19937 g(rd());
            shuffle(candidates.begin(), candidates.end(), g);
            for (int j : candidates) {
                pot2[idx2].t5 = pot1[j].name;
                pot1[j].t6 = pot2[idx2].name;
                if (lookaheadv2(pot1, pot2, allTeams)) {
                    if (assignMatchesv2(pot1, pot2, allTeams, idx1, idx2))
                        return true;
                }
                pot2[idx2].t5 = "";
                pot1[j].t6 = "";
            }
            return false;
        }
        if (pot2[idx2].t6 == "") {
            vector<int> candidates;
            for (int i = 0; i < pot1.size(); ++i) {
                if (pot1[i].t5 == "" &&
                    pot2[idx2].name != pot1[i].name &&
                    pot2[idx2].country != pot1[i].country &&
                    pot2[idx2].t5 != pot1[i].name &&
                    pot1[i].t5 != pot2[idx2].name &&
                    pot1[i].t6 != pot2[idx2].name &&
                    !is2(pot2[idx2], allTeams, pot1[i].country) &&
                    !is2(pot1[i], allTeams, pot2[idx2].country))
                    candidates.push_back(i);
            }
            random_device rd; mt19937 g(rd());
            shuffle(candidates.begin(), candidates.end(), g);
            for (int j : candidates) {
                pot2[idx2].t6 = pot1[j].name;
                pot1[j].t5 = pot2[idx2].name;
                if (lookaheadv2(pot1, pot2, allTeams)) {
                    if (assignMatchesv2(pot1, pot2, allTeams, idx1, idx2))
                        return true;
                }
                pot2[idx2].t6 = "";
                pot1[j].t5 = "";
            }
            return false;
        }
    }
    return true;
}

bool assignMatchesv3(vector<CalendarTeam>& pot1, vector<CalendarTeam>& pot2, const vector<CalendarTeam>& allTeams, int idx1 = 0, int idx2 = 0) {
    for (; idx1 < pot1.size(); ++idx1) {
        if (pot1[idx1].t7 == "") {
            vector<int> candidates;
            for (int i = 0; i < pot2.size(); ++i) {
                if (pot2[i].t8 == "" &&
                    pot1[idx1].name != pot2[i].name &&
                    pot1[idx1].country != pot2[i].country &&
                    pot1[idx1].t8 != pot2[i].name &&
                    pot2[i].t7 != pot1[idx1].name &&
                    pot2[i].t8 != pot1[idx1].name &&
                    !is2(pot1[idx1], allTeams, pot2[i].country) &&
                    !is2(pot2[i], allTeams, pot1[idx1].country))
                    candidates.push_back(i);
            }
            random_device rd; mt19937 g(rd());
            shuffle(candidates.begin(), candidates.end(), g);
            for (int j : candidates) {
                pot1[idx1].t7 = pot2[j].name;
                pot2[j].t8 = pot1[idx1].name;
                if (lookaheadv3(pot1, pot2, allTeams)) {
                    if (assignMatchesv3(pot1, pot2, allTeams, idx1, idx2))
                        return true;
                }
                pot1[idx1].t7 = "";
                pot2[j].t8 = "";
            }
            return false;
        }
        if (pot1[idx1].t8 == "") {
            vector<int> candidates;
            for (int i = 0; i < pot2.size(); ++i) {
                if (pot2[i].t7 == "" &&
                    pot1[idx1].name != pot2[i].name &&
                    pot1[idx1].country != pot2[i].country &&
                    pot1[idx1].t7 != pot2[i].name &&
                    pot2[i].t7 != pot1[idx1].name &&
                    pot2[i].t8 != pot1[idx1].name &&
                    !is2(pot1[idx1], allTeams, pot2[i].country) &&
                    !is2(pot2[i], allTeams, pot1[idx1].country))
                    candidates.push_back(i);
            }
            random_device rd; mt19937 g(rd());
            shuffle(candidates.begin(), candidates.end(), g);
            for (int j : candidates) {
                pot1[idx1].t8 = pot2[j].name;
                pot2[j].t7 = pot1[idx1].name;
                if (lookaheadv3(pot1, pot2, allTeams)) {
                    if (assignMatchesv3(pot1, pot2, allTeams, idx1, idx2))
                        return true;
                }
                pot1[idx1].t8 = "";
                pot2[j].t7 = "";
            }
            return false;
        }
    }
    for (; idx2 < pot2.size(); ++idx2) {
        if (pot2[idx2].t7 == "") {
            vector<int> candidates;
            for (int i = 0; i < pot1.size(); ++i) {
                if (pot1[i].t8 == "" &&
                    pot2[idx2].name != pot1[i].name &&
                    pot2[idx2].country != pot1[i].country &&
                    pot2[idx2].t8 != pot1[i].name &&
                    pot1[i].t7 != pot2[idx2].name &&
                    pot1[i].t8 != pot2[idx2].name &&
                    !is2(pot2[idx2], allTeams, pot1[i].country) &&
                    !is2(pot1[i], allTeams, pot2[idx2].country))
                    candidates.push_back(i);
            }
            random_device rd; mt19937 g(rd());
            shuffle(candidates.begin(), candidates.end(), g);
            for (int j : candidates) {
                pot2[idx2].t7 = pot1[j].name;
                pot1[j].t8 = pot2[idx2].name;
                if (lookaheadv3(pot1, pot2, allTeams)) {
                    if (assignMatchesv3(pot1, pot2, allTeams, idx1, idx2))
                        return true;
                }
                pot2[idx2].t7 = "";
                pot1[j].t8 = "";
            }
            return false;
        }
        if (pot2[idx2].t8 == "") {
            vector<int> candidates;
            for (int i = 0; i < pot1.size(); ++i) {
                if (pot1[i].t7 == "" &&
                    pot2[idx2].name != pot1[i].name &&
                    pot2[idx2].country != pot1[i].country &&
                    pot2[idx2].t7 != pot1[i].name &&
                    pot1[i].t7 != pot2[idx2].name &&
                    pot1[i].t8 != pot2[idx2].name &&
                    !is2(pot2[idx2], allTeams, pot1[i].country) &&
                    !is2(pot1[i], allTeams, pot2[idx2].country))
                    candidates.push_back(i);
            }
            random_device rd; mt19937 g(rd());
            shuffle(candidates.begin(), candidates.end(), g);
            for (int j : candidates) {
                pot2[idx2].t8 = pot1[j].name;
                pot1[j].t7 = pot2[idx2].name;
                if (lookaheadv3(pot1, pot2, allTeams)) {
                    if (assignMatchesv3(pot1, pot2, allTeams, idx1, idx2))
                        return true;
                }
                pot2[idx2].t8 = "";
                pot1[j].t7 = "";
            }
            return false;
        }
    }
    return true;
}

bool lookaheadSAMEPOT(const vector<CalendarTeam>& teams, const vector<CalendarTeam>& allTeams) {
    for (int i = 0; i < teams.size(); ++i) {
        const auto& team = teams[i];
        if (!team.t1.empty() && team.t1 == team.t2) return false;
        if (team.t1 == "") {
            auto v = findPossibleOpponentsSAMEPOT(team, teams, allTeams, "t1");
            if (v.empty()) return false;
        }
        if (team.t2 == "") {
            auto v = findPossibleOpponentsSAMEPOT(team, teams, allTeams, "t2");
            if (v.empty()) return false;
        }
    }
    for (int i = 0; i < teams.size(); ++i) {
        for (int j = i+1; j < teams.size(); ++j) {
            const auto& t1 = teams[i], &t2 = teams[j];
            if ((t1.t1 == t2.name && t2.t1 == t1.name) ||
                (t1.t2 == t2.name && t2.t2 == t1.name))
                return false;
        }
    }
    return true;
}

bool assignMatchesBT(vector<CalendarTeam>& teams, const vector<CalendarTeam>& allTeams, int idx = 0) {
    if (idx == teams.size()) {
        for (const auto& t : teams)
            if (t.t1 == "" || t.t2 == "")
                return false;
        for (int i = 0; i < teams.size(); ++i) {
            for (int j = i+1; j < teams.size(); ++j) {
                const auto& t1 = teams[i], &t2 = teams[j];
                if ((t1.t1 == t2.name && t2.t1 == t1.name) ||
                    (t1.t2 == t2.name && t2.t2 == t1.name))
                    return false;
            }
        }
        return true;
    }
    vector<int> teamOrder;
    for (int i = idx; i < teams.size(); ++i) teamOrder.push_back(i);
    sort(teamOrder.begin(), teamOrder.end(), [&](int a, int b){
        return teams[a].matchesAssigned() < teams[b].matchesAssigned();
    });
    for (int ord = 0; ord < teamOrder.size(); ++ord) {
        int tIdx = teamOrder[ord];
        auto& team = teams[tIdx];
        if (team.t1 != "" && team.t2 != "") continue;
        if (team.t1 == "") {
            auto possibles = findPossibleOpponentsSAMEPOT(team, teams, allTeams, "t1");
            for (int i : possibles) {
                if (teams[i].t2 != "") continue;
                team.t1 = teams[i].name;
                teams[i].t2 = team.name;
                if (lookaheadSAMEPOT(teams, allTeams) && assignMatchesBT(teams, allTeams, idx+1)) return true;
                team.t1 = "";
                teams[i].t2 = "";
            }
        }
        if (team.t2 == "") {
            auto possibles = findPossibleOpponentsSAMEPOT(team, teams, allTeams, "t2");
            for (int i : possibles) {
                if (teams[i].t1 != "") continue;
                team.t2 = teams[i].name;
                teams[i].t1 = team.name;
                if (lookaheadSAMEPOT(teams, allTeams) && assignMatchesBT(teams, allTeams, idx+1)) return true;
                team.t2 = "";
                teams[i].t1 = "";
            }
        }
        return false;
    }
    return assignMatchesBT(teams, allTeams, idx+1);
}

int main() {
    vector<CalendarTeam> pot1data = {
        {"PSG", "France"},
        {"Dortmund", "Germany"},
        {"Liverpool", "England"},
        {"Chelsea", "England"},
        {"Barcelona", "Spain"},
        {"Real Madrid", "Spain"},
        {"Bayern Munich", "Germany"},
        {"Inter Milan", "Italy"},
        {"Manchester City", "England"}
    };

    vector<CalendarTeam> pot2data = {
        {"Ajax", "Netherlands"},
        {"Arsenal", "England"},
        {"Atletico Madrid", "Spain"},
        {"Juventus", "Italy"},
        {"Villarreal", "Spain"},
        {"Sporting CP", "Portugal"},
        {"Club Bruges", "Belgium"},
        {"PSV", "Netherlands"},
        {"Eintracht Frankfurt", "Germany"}
    };
    vector<CalendarTeam> pot3data = {
        {"Atalanta", "Italy"},
        {"Tottenham", "England"},
        {"Napoli", "Italy"},
        {"Marseille", "France"},
        {"Galatasaray", "Turkey"},
        {"Basel", "Switzerland"},
        {"Slavia Praga", "Czech Republic"},
        {"Bayer Leverkusen", "Germany"},
        {"Olympiacos", "Greece"}
    };

    vector<CalendarTeam> pot4data = {
        {"Qarabag", "Azerbaijan"},
        {"Copenhagen", "Denmark"},
        {"Athletic Bilbao", "Spain"},
        {"Dinamo Kyiv", "Ukraine"},
        {"Monaco", "France"},
        {"Union SG", "Belgium"},
        {"Salzburg", "Austria"},
        {"Newcastle", "England"},
        {"Lech Poznan", "Poland"}
    };

    vector<CalendarTeam> allTeams;
    allTeams.insert(allTeams.end(), pot1data.begin(), pot1data.end());
    allTeams.insert(allTeams.end(), pot2data.begin(), pot2data.end());
    allTeams.insert(allTeams.end(), pot3data.begin(), pot3data.end());
    allTeams.insert(allTeams.end(), pot4data.begin(), pot4data.end());

    bool ok = assignMatchesBT(pot1data, allTeams);
    if (!ok) {
        cout << "Could not assign matches for all teams without deadlock. POT1 \n";
        return 1;
    }

    cout << "Matches assigned for Pot 1 teams without deadlock.\n";
    
    bool ok1 = assignMatches(pot1data, pot2data, allTeams);
    if (!ok1) {
        cout << "No valid assignment possible!\n";
        return 1;
    }
    cout << "Matches assigned for Pot 1 and Pot 2 teams without deadlock.\n";
     
    //cout << "Matches assigned for Pot 1 and Pot 4 teams without deadlock.\n";
    bool ok2 = assignMatchesv2(pot1data, pot3data, allTeams);
    if (!ok2) {
        cout << "Could not assign matches for pot 1 and pot 3 teams without deadlock.\n";
        return 1;
    }
    cout << "Matches assigned for Pot 1 and Pot 3 teams without deadlock.\n";

    bool ok3 = false;
    auto start = chrono::steady_clock::now();
    do {
        ok3 = assignMatchesv3(pot1data, pot4data, allTeams);
        if (!ok3) {
            // Re-run ok2 before retrying ok3
            //resetting previous assignments
            for (auto& t : pot1data) {
                t.reset();
            }
            for (auto& t : pot3data) {
                t.reset();
            }
            for (auto& t : pot4data) {
                t.reset();
            }

            
            ok2 = assignMatchesv2(pot1data, pot3data, allTeams);
            if (!ok2) {
                cout << "Could not assign matches for pot 1 and pot 3 teams without deadlock (on retry).\n";
                return 1;
            }
        }
        auto now = chrono::steady_clock::now();
        if (chrono::duration_cast<chrono::seconds>(now - start).count() > 10) {
            cout << "Timeout: Could not assign matches for pot 1 and pot 4 teams without deadlock after several retries.\n";
            return 1;
        }
    } while (!ok3);

    cout << "Matches assigned for Pot 1 and Pot 4 teams without deadlock.\n";

    

    bool ok4 = assignMatchesBT(pot2data, allTeams);
    if (!ok4) {
        cout << "Could not assign matches for additional teams without deadlock. POT2 \n";
        return 1;
    }
    cout << "Matches assigned for Pot 2 teams without deadlock.\n";

    bool ok5 = assignMatchesv3(pot2data, pot3data, allTeams);
    if (!ok5) {
        cout << "Could not assign matches for pot 2 and pot 3 teams without deadlock. POT2 AND 3\n";
        return 1;
    }
    cout << "Matches assigned for Pot 2 and Pot 3 teams without deadlock.\n";

    bool ok6 = assignMatchesv2(pot2data, pot4data, allTeams);
    if (!ok6) {
        cout << "Could not assign matches for pot 2 and pot 4 teams without deadlock.\n";
        return 1;
    }
    cout << "Matches assigned for Pot 2 and Pot 4 teams without deadlock.\n";

    bool ok7 = assignMatchesBT(pot3data, allTeams);
    if (!ok7) {
        cout << "Could not assign matches for pot 3 teams without deadlock.\n";
        return 1;
    }
    cout << "Matches assigned for Pot 3 teams without deadlock.\n";
    bool ok8 = assignMatches(pot3data, pot4data, allTeams);
    if (!ok8) {
        cout << "Could not assign matches for pot 3 and pot 4 teams without deadlock.\n";
        return 1;
    }
    cout << "Matches assigned for Pot 3 and Pot 4 teams without deadlock.\n";
    bool ok9 = assignMatchesBT(pot4data, allTeams);
    if (!ok9) {
        cout << "Could not assign matches for pot 4 teams without deadlock.\n";
        return 1;
    }
    cout << "Matches assigned for Pot 4 teams without deadlock.\n";

    cout << "Champions League Matches (Pot 1):\n";
    for (const auto& t : pot1data) {
        t.print1();
    }
    cout << endl;
    cout << "Champions League Matches (Pot 2):\n";
    for (const auto& t : pot2data) {
        t.print2();
    }
    cout << endl;
    cout << "Champions League Matches (Pot 3):\n";
    for (const auto& t : pot3data) {
        t.print3();
    }
    cout << endl;
    cout << "Champions League Matches (Pot 4):\n";
    for (const auto& t : pot4data) {
        t.print4();
    }
    return 0;
}
