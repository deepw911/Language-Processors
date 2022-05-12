#include <bits/stdc++.h>
using namespace std;

vector<string> terminals;
vector<string> nonterminals;
unordered_map<string, vector<vector<string>>> productions;

unordered_map<string, vector<string>> first;
unordered_map<string, vector<string>> follow;
map<int, vector<vector<string>>> productionsMap;
typedef pair<vector<vector<string>>, vector<string>> item;
// in item above: vector<vector<string>> is the production, vector<string> is the lookahead
vector<vector<item>> canonicalItems;
vector<vector<string>> parseTableCLR;
vector<vector<string>> parseTableLALR;
vector<vector<int>> mergeIndex;
set<int> indexSet;
bool isLALR = true;
bool srConflict = false;
int srCount = 0;
bool rrConflict = false;
int rrCount = 0;
int clrConflicts = 0;

void displayProductions()
{
    cout << "\nProductions" << endl;
    for (auto &i : productions)
    {
        cout << i.first << "->";
        for (auto &j : i.second)
        {
            for (auto &k : j)
            {
                cout << k << " ";
            }
            cout << "| ";
        }
        cout << endl;
    }
}

void displayFirst()
{
    cout << "First Set\n";
    for (auto &i : first)
    {
        cout << i.first << " -> ";
        for (auto &j : i.second)
        {
            cout << j << " ";
        }
        cout << endl;
    }
}

void displayFollow()
{
    cout << "Follow Set\n";
    for (auto &i : follow)
    {
        cout << i.first << " -> ";
        for (auto &j : i.second)
        {
            cout << j << " ";
        }
        cout << endl;
    }
}

void getFirst(string s)
{
    bool flag = 1;
    for (auto &i : productions[s])
    {
        for (auto &p : i)
        {
            if (flag && find(begin(terminals), end(terminals), p) != terminals.end())
            {
                if (find(begin(first[s]), end(first[s]), p) == first[s].end())
                {
                    first[s].push_back(p);
                }
                flag = 0;
                break;
            }
            else if (flag && find(begin(nonterminals), end(nonterminals), p) != nonterminals.end())
            {
                flag = 0;
                if (first.find(p) == first.end())
                {
                    getFirst(p);
                }
                for (auto &k : first[p])
                {
                    if (k != string(1, 'e') && find(begin(first[s]), end(first[s]), k) == first[s].end())
                    {
                        first[s].push_back(k);
                    }
                    if (k == string(1, 'e'))
                    {
                        flag = 1;
                    }
                }
                if (!flag)
                {
                    break;
                }
            }
            else
            {
                if (find(begin(first[s]), end(first[s]), p) == first[s].end())
                {
                    first[s].push_back(p);
                }
            }
        }
        if (flag)
        {
            if (find(begin(first[s]), end(first[s]), string(1, 'e')) == first[s].end())
            {
                first[s].push_back(string(1, 'e'));
            }
        }
    }
}

void getFollow(string s)
{
    bool flag = 0;
    for (auto x : productions)
    {
        for (auto &i : x.second)
        {
            for (auto &p : i)
            {
                if (p == s)
                {
                    flag = 1;
                    continue;
                }
                if (flag && find(begin(nonterminals), end(nonterminals), p) != nonterminals.end())
                {
                    flag = 0;
                    for (auto k : first[p])
                    {
                        if (k != string(1, 'e'))
                        {
                            if (find(begin(follow[s]), end(follow[s]), k) == follow[s].end())
                            {
                                follow[s].push_back(k);
                            }
                        }
                        else
                        {
                            flag = 1;
                        }
                    }
                }
                else if (flag && find(begin(terminals), end(terminals), p) != terminals.end())
                {
                    if (find(begin(follow[s]), end(follow[s]), p) == follow[s].end())
                    {
                        follow[s].push_back(p);
                    }
                    flag = 0;
                }
            }
            if (flag)
            {
                if (follow.find(x.first) == follow.end())
                {
                    getFollow(x.first);
                }
                for (auto k : follow[x.first])
                {
                    if (find(begin(follow[s]), end(follow[s]), k) == follow[s].end())
                    {
                        follow[s].push_back(k);
                    }
                }
            }
        }
    }
}

void getProductionsMap()
{
    int index = 0;
    for (auto x : productions)
    {
        vector<vector<string>> v;
        v.push_back({x.first}); // pushing the vector with string s

        for (auto i : x.second)
        {
            v.push_back(i);
            productionsMap[index] = v;
            index++;
            v.pop_back();
        }
    }
    for (auto x : productionsMap)
    {
        cout << x.first << "  " << x.second[0][0] << " -> ";
        for (auto i : x.second[1])
        {
            cout << i;
        }
        cout << endl;
    }
}

string getClosure(vector<item> it)
{
    vector<item> closure;
    int row = canonicalItems.size();
    // initializing the rows of parse table with a vector of size nonterminals.size()+terminals.size()
    parseTableCLR.push_back(vector<string>(nonterminals.size() + terminals.size(), "   "));
    for (auto itr : it)
    {
        // storing production map in p
        vector<vector<string>> p = itr.first;
        // getting lookaheads from item
        vector<string> lookaheads = itr.second;
        // symbol is a start state string
        string symbol = p[0][0];
        // setting production to the starting productio
        // at index 0 of p S' is there so setting production to (S)
        vector<string> production = p[1];
        closure.push_back(itr);
        queue<vector<vector<string>>> q; // if nonterminal to expand has nonterminals as start of its productions
        q.push(p);
        while (!q.empty())
        {
            vector<vector<string>> ele = q.front();
            production = ele[1];
            q.pop();
            int i = 0;
            while (production[i] != ".")
            {
                i++;
            }
            i++;

            // if "." is at the end or  no "." found
            if (i == production.size())
            {
                // if last string in non terminals is equals to symbol then the state is accepted
                if (nonterminals.back() == symbol)
                {
                    parseTableCLR[canonicalItems.size()][terminals.size()] = "acc"; // acc is accepted state
                }
                else
                {
                    // finding which production to reduce to
                    int pos = -1;
                    production.pop_back();
                    for (auto itr2 : productionsMap)
                    {
                        // setting the pos
                        if (itr2.second[0][0] == symbol && itr2.second[1] == production)
                        {
                            pos = itr2.first;
                        }
                    }
                    if (pos != -1)
                    {
                        for (auto itr : lookaheads)
                        {
                            // if lookahead is "$" then pos will be same as calculated above.
                            // else pos will be calculated for the lookahead and added to the table
                            int posTerminal;
                            if (itr == "$")
                            {
                                posTerminal = terminals.size();
                            }
                            else
                            {
                                posTerminal = find(terminals.begin(), terminals.end(), itr) - terminals.begin();
                            }
                            if (parseTableCLR[row][posTerminal] == "   " || parseTableCLR[row][posTerminal] == "r" + to_string(pos) + " ")
                                parseTableCLR[row][posTerminal] = "r" + to_string(pos) + " ";
                            else
                                clrConflicts++;
                        }
                    }
                }
                // setting i back to 0
                i = 0;
            }
            else
            {
                // checking here if the next element after "." is a non terminal
                if (find(nonterminals.begin(), nonterminals.end(), production[i]) != nonterminals.end())
                {
                    item itm;
                    vector<string> st; // for lookaheads
                    vector<vector<string>> temp;
                    vector<string> v(1, production[i]);
                    st.clear();
                    temp.push_back(v);
                    // if we are at the last index of production then we will assign lookaheads to the vector st
                    // taking care of lookaheads
                    if (i + 1 == production.size())
                        st = lookaheads;
                    else
                    {
                        // checking if next index of production has a terminal
                        if (find(terminals.begin(), terminals.end(), production[i + 1]) != terminals.end())
                        {
                            // if it is a terminal then we will add it to st
                            if (find(st.begin(), st.end(), production[i + 1]) == st.end())
                            {
                                st.push_back(production[i + 1]);
                            }
                        }
                        else
                        { // if next index is non terminal
                            int flag = 0;
                            // as we need to get the first of non-terminals
                            vector<string> st2 = first[production[i + 1]];
                            for (auto itr : st2)
                            {
                                // if first is not an epsilon then push it to st
                                if (itr != "e")
                                {
                                    if (find(st.begin(), st.end(), itr) == st.end())
                                    {
                                        st.push_back(itr);
                                    }
                                }
                                else
                                { // first is episilon
                                    flag = 1;
                                }
                            }
                            // if we get an epsilon then we have to check for (i+2)th production
                            int j = i + 2;
                            while (flag && j < production.size())
                            {
                                flag = 0;
                                // again getting the first for (i+2)th production and so on
                                st2 = first[production[j]];
                                for (auto itr : st2)
                                {
                                    // if first is not an epsilon then push it to st
                                    if (itr != "e")
                                    {
                                        if (find(st.begin(), st.end(), itr) == st.end())
                                        {
                                            st.push_back(itr);
                                        }
                                    }
                                    else
                                    {
                                        // first is episilon continue the loop till we get a non epsilon or production ends
                                        flag = 1;
                                    }
                                }
                                j++;
                            }

                            if (flag)
                            {
                                // if an epsilon was encountered then we will push lookaheads to st.
                                for (auto itr : lookaheads)
                                {
                                    if (find(st.begin(), st.end(), itr) == st.end())
                                    {
                                        st.push_back(itr);
                                    }
                                }
                            }
                        }
                    }

                    // iterating through productions with ith string of production
                    for (auto it2 : productions[production[i]])
                    {
                        vector<string> v2 = it2;
                        v.clear();
                        v.push_back(".");
                        // here we are storing the production in v
                        for (auto it3 : v2)
                        {
                            if (it3 != "e")
                                v.push_back(it3);
                        }
                        temp.push_back(v);
                        // here we made a item with the production and lookaheads and pushed it to closure
                        closure.push_back(make_pair(temp, st));
                        temp.pop_back();
                    }
                }
            }
        }
    }

    string returnString = "";

    // HERE WE ARE DECIDING THE SHIFT STATES
    // here we are checking if closure items are already present in canonical items or not

    if (find(canonicalItems.begin(), canonicalItems.end(), closure) == canonicalItems.end())
    {
        canonicalItems.push_back(closure);
        // setting the shift state to 1+ to number of items we have
        //  so row is the size of vector of canonical items.
        returnString = "s" + to_string(row) + " ";
    }
    else
    {
        // if closure is already present in canonical items then we will set the shift state to the index of closure
        int pos = find(canonicalItems.begin(), canonicalItems.end(), closure) - canonicalItems.begin();
        parseTableCLR.pop_back(); // no need of row which we inserted at start
        return "s" + to_string(pos) + " ";
    }

    // dfs to getClosure of next elements
    set<string> symbols; // to store symbols where we will call dfs
    vector<item> items;
    // here we are getting the items from closure
    for (auto it2 = closure.begin(); it2 != closure.end(); it2++)
    {
        item itm = *it2;
        vector<vector<string>> temp;
        temp.push_back(itm.first[0]);
        vector<string> p2 = itm.first[1];
        int i = 0;
        while (p2[i] != ".")
        {
            i++;
        }

        if (p2.size() == 1)
        { // case of S->e (where e is epsilon)
            vector<string> st = itm.second;
            temp.push_back(p2);
            temp[1][0] = "e";
            int posOfProduction = -1;
            for (auto itr2 : productionsMap)
            {
                // if production is found then we will set the posOfProduction to the ley of the productionsMap
                if (itr2.second[0][0] == temp[0][0] && itr2.second[1] == temp[1])
                {
                    posOfProduction = itr2.first;
                }
            }
            for (auto itr : st)
            {
                // getting the index of itr from terminals
                int pos = find(terminals.begin(), terminals.end(), itr) - terminals.begin();
                // setting the reduced state to the index of the itr
                if (parseTableCLR[row][pos] == "   " || parseTableCLR[row][pos] == "r" + to_string(posOfProduction) + " ")
                {
                    parseTableCLR[row][pos] = "r" + to_string(posOfProduction) + " ";
                }
                else
                { // if already filled increase conflict
                    clrConflicts++;
                }
            }
        }
        else if (i < p2.size() - 1 && symbols.find(p2[i + 1]) == symbols.end())
        {
            // if lookahead is not present in symbols then we will add it to symbols
            items.clear();
            p2[i] = p2[i + 1];
            p2[i + 1] = ".";
            string sym = p2[i];
            // adding it to the symbol
            symbols.insert(sym);
            temp.push_back(p2);
            itm = make_pair(temp, itm.second);
            items.push_back(itm);
            // this loop is to check if multiple productions can transition on the same symbol to diff state
            for (auto it3 = it2 + 1; it3 != closure.end(); it3++)
            {
                item itm2 = *it3;
                temp.clear();
                temp.push_back(itm2.first[0]);
                vector<string> p3 = itm2.first[1];
                int j = 0;
                while (p3[j] != ".")
                {
                    j++;
                }
                if (p3[j + 1] == sym)
                {
                    p3[j] = p3[j + 1];
                    p3[j + 1] = ".";
                    temp.push_back(p3);
                    itm2 = make_pair(temp, itm2.second);
                    items.push_back(itm2);
                }
            }
            // callig the getClosure again with the items
            string ret = getClosure(items);
            // if symbol sym is present in terminals
            // set the reduced value(r4,r5 etc to the parse table)
            if (find(terminals.begin(), terminals.end(), sym) != terminals.end())
            {
                int pos = find(terminals.begin(), terminals.end(), sym) - terminals.begin();

                if (parseTableCLR[row][pos] == "   " || parseTableCLR[row][pos] == ret)
                {
                    parseTableCLR[row][pos] = ret;
                }
                else
                {
                    clrConflicts++;
                }
            }
            else
            {
                // if it is not in terminals then find the index in non-terminals
                // and "ret" to parseTableCLR
                int pos = find(nonterminals.begin(), nonterminals.end(), sym) - nonterminals.begin();
                ret.erase(0, 1);
                if (parseTableCLR[row][terminals.size() + 1 + pos] == "   " || parseTableCLR[row][terminals.size() + 1 + pos] == ret)
                    parseTableCLR[row][terminals.size() + 1 + pos] = ret;
                else
                    clrConflicts++;
            }
        }
    }
    return returnString;
}

void getCLRTable()
{
    // CLR parse table
    cout << "CLR Parse Table : " << endl;
    cout << "State ";
    for (auto &i : terminals)
    {
        cout << i << "    ";
    }

    cout << "$    ";
    for (auto &i : nonterminals)
    {
        cout << i << "    ";
    }
    cout << endl;

    for (int i = 0; i < parseTableCLR.size(); i++)
    {
        cout << i << "     ";
        for (int j = 0; j < parseTableCLR[i].size(); j++)
        {
            cout << parseTableCLR[i][j] << "  ";
        }
        cout << endl;
    }
}

vector<int> check(item itm, int index)
{
    // check function to get the reduced items which can be merged
    int ind = 0;
    vector<int> v;

    for (auto it1 : canonicalItems)
    {
        if (ind == 0)
        {
            ind++;
            continue;
        }
        for (auto it2 : it1)
        {
            if (it2.first == itm.first)
            {
                v.push_back(ind);
            }
        }
        ind++;
    }
    return v;
}

void lalrParser()
{
    int index = 0;
    vector<int> v;
    for (auto it1 : canonicalItems)
    {
        // checking if index is already present in this set ot not
        if (indexSet.find(index) == indexSet.end())
        {
            index++;
            continue;
        }
        for (auto it2 : it1)
        {
            // calling the check function to check if any other reduced state has same production or not
            v = check(it2, index);
            if (v.size() > 1)
            {
                sort(v.begin(), v.end());
                // mergeIndex is basically a vector of vector where every vector contains indexes that has to be merged
                if (find(mergeIndex.begin(), mergeIndex.end(), v) == mergeIndex.end())
                {
                    mergeIndex.push_back(v);
                }
            }
        }
        index++;
    }
}

void getLALRTable()
{
    // this vector will help us in printing which states got merged in lalr table
    vector<int> indexLALR;
    int i = 0;
    // duplicating the CLR table to LALR
    for (auto it : parseTableCLR)
    {
        indexLALR.push_back(i);
        parseTableLALR.push_back(it);
        i++;
    }
    // count is basically the number of extra rows in the end of LALR table that needs to be removed
    int count = -mergeIndex.size();
    for (int i = 0; i < mergeIndex.size(); i++)
    {
        int ind = mergeIndex[i][0];
        count += mergeIndex[i].size();
        int temp = indexLALR[ind];
        for (int j = 1; j < mergeIndex[i].size(); j++)
        {
            temp = temp * 10 + mergeIndex[i][j];
            indexLALR.erase(indexLALR.begin() + mergeIndex[i][j]);
            // here we are merging rows which have same production map
            for (int k = 0; k < parseTableLALR[ind].size(); k++)
            {
                // checking for rr conflict we a
                if (parseTableLALR[ind][k][0] == 'r')
                {
                    if (parseTableLALR[mergeIndex[i][j]][k][0] == 'r')
                    {
                        rrConflict = true;
                        rrCount++;
                    }
                    else if (parseTableLALR[mergeIndex[i][j]][k][0] == 's')
                    {
                        srConflict = true;
                        srCount++;
                    }
                    parseTableLALR[ind][k] += parseTableLALR[mergeIndex[i][j]][k];
                }
                else if (parseTableLALR[ind][k][0] == 's') // sr conflict check
                {
                    if (parseTableLALR[mergeIndex[i][j]][k][0] == 'r')
                    {
                        srConflict = true;
                        srCount++;
                    }
                    parseTableLALR[ind][k] += parseTableLALR[mergeIndex[i][j]][k];
                }
                else
                {
                    parseTableLALR[ind][k] = parseTableLALR[mergeIndex[i][j]][k];
                }
            }
        }
        indexLALR[ind] = temp;
    }
    // popping all the extra layers at the end of LALR table
    while (count--)
        parseTableLALR.pop_back();

    // printing the LALR Table

    cout << "LALR Table : " << endl;
    cout << "State ";
    for (auto &i : terminals)
    {
        cout << i << "    ";
    }

    cout << "$    ";
    for (auto &i : nonterminals)
    {
        cout << i << "    ";
    }
    cout << endl;

    for (int i = 0; i < parseTableLALR.size(); i++)
    {
        cout << indexLALR[i] << "     ";
        for (int j = 0; j < parseTableLALR[i].size(); j++)
        {
            cout << parseTableLALR[i][j] << "  ";
        }
        cout << endl;
    }
}

int main()
{
    int i, j;
    ifstream file;
    file.open("input.txt");
    string line;
    if (file.is_open())
    {
        while (getline(file, line))
        {
            if (line.substr(0, 9) == "TERMINALS")
            {
                string temp;
                for (i = 10; i < line.size(); i++)
                {
                    if (line[i] == ' ')
                    {
                        terminals.push_back(temp);
                        temp.clear();
                    }
                    else
                        temp += line[i];
                }
                terminals.push_back(temp);
            }
            else if (line.substr(0, 12) == "NONTERMINALS")
            {
                string temp;
                for (i = 13; i < line.size(); i++)
                {
                    if (line[i] == ' ')
                    {
                        nonterminals.push_back(temp);
                        temp.clear();
                    }
                    else
                        temp += line[i];
                }
                nonterminals.push_back(temp);
            }
            else
            {
                vector<string> prod;
                string temp = line.substr(5); // a -> b (so b is 5th index)
                // int i=0;
                for (auto i : temp)
                {
                    if (i != ' ')
                        prod.push_back(string(1, i)); // pushing the string into prod vector
                    // string(1,i) is used to convert char to string i.e. 'a' to "a"
                    // string(1,i) means 1 times i in a string
                }
                // pushing the production into productions map
                // and using .substr function because our map takes string as key not char
                productions[line.substr(0, 1)].push_back(prod);
            }
        }
    }

    file.close();
    // follow of first state = $
    follow[nonterminals[0]].push_back(string(1, '$'));

    // displaying terminals and non-terminals
    cout << "Terminals: \n";
    for (auto &i : terminals)
    {
        cout << i << " ";
    }

    cout << "\nNon Terminals" << endl;
    for (auto &i : nonterminals)
    {
        cout << i << " ";
    }

    // displaying productions
    displayProductions();

    // calling getFirst function, to get the first values of all nonterminals
    for (auto &i : nonterminals)
    {
        getFirst(i);
    }

    displayFirst();

    // calling getFollow function, to get the follow values of all nonterminals
    for (auto &i : nonterminals)
    {
        getFollow(i);
    }

    displayFollow();

    // Augmenting grammar
    string firstNT = nonterminals[0];
    string augmentStart = firstNT + "'";                   // to make S to S'
    nonterminals.push_back(augmentStart);                  // adding S' to nonterminals
    productions[augmentStart].push_back(vector<string>()); // adding empty string to  make :S'->
    productions[augmentStart][0].push_back(firstNT);       // adding S to S'

    // displaying augmented productions
    cout << "\n\nAugmented grammar productions : " << endl;
    getProductionsMap();

    vector<vector<string>> temp;
    temp.push_back({augmentStart});
    temp.push_back({".", firstNT});

    vector<string> st = {"$"};
    // item it=make_pair(temp,st);
    vector<item> items;
    // PUSHING S'-> .S to items
    items.push_back(make_pair(temp, st));

    getClosure(items);

    cout << "\n\n";

    // to print CLR table
    getCLRTable();

    cout << "\n\n";

    // Printing all the items
    cout << "Canonical Items : \n";
    int index = 0;
    for (auto it1 : canonicalItems)
    {
        cout << "State " << index << " \n";
        string prev;
        bool flag = true;
        for (auto it2 : it1)
        {
            for (auto it3 : it2.first)
            {
                for (auto it4 : it3)
                {
                    cout << it4 << " ";
                    prev = it4;
                }
            }
            cout << ":";
            for (auto it4 : it2.second)
            {
                cout << it4 << " ";
            }
            cout << "\n";
            if (prev != ".")
            {
                flag = false;
            }
            // cout<<index<<" "<<prev<<"\n";
        }
        if (flag)
        {
            indexSet.insert(index);
        }
        index++;
    }

    // finding reduced states which can be merged
    lalrParser();

    cout << "\n\n";

    // converting CLR table to LALR table
    getLALRTable();

    cout << "\n\n";

    if (clrConflicts > 0)
    {
        cout << "\nThis is not a CLR Grammar as there are " << clrConflicts << " conflicts\n\n";
        cout << "Therefore, The given grammar is not LALR\n";
    }
    else
    {
        cout << "Number of S R Conflicts: " << srCount << endl;
        cout << "Number of R R Conflicts: " << rrCount << endl;

        if (!srConflict && !rrConflict)
            cout << "The given grammar is LALR\n";
        else
            cout << "The given grammar is not LALR\n";
    }
    return 0;
}
