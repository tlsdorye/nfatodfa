#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <set>
#include <map>
#include <utility>
#include <algorithm>

using namespace std;
typedef vector<int> State;

class ConvertDFA
{
public:
	vector<State> states;
	int input_size;
	vector<int> start_state;
	vector<State> final_states;
	map<pair<State, int>, State> mapping_function;
	map<State, int> state_idx;

	ConvertDFA() {};
	ConvertDFA(int input_size)
	{
		this->input_size = input_size;
	}

	void FindFinalState(State state, State n_final_state)
	{
		for (int i = 0; i < state.size(); i++)
			for (int j = 0; j < n_final_state.size(); j++)
				if (state[i] == n_final_state[j])
				{
					final_states.push_back(state);
					return;
				}
	}

	void PrintConvertDFA()
	{
		cout << "\n\n";
		cout << "-------------------+-----------+---------------------\n";
		cout << "################### CONVERT DFA #####################\n";
		cout << "-------------------+-----------+---------------------\n";
		cout << "\n";

		cout << "\n STATES OF DFA :\t\t";
		for (int i = 0; i < states.size(); i++)
		{
			for (int j = 0; j < states[i].size(); j++) cout << (char)(states[i][j] + 'A');
			cout << ", ";
		}
		cout << "\n";

		cout << "\n INPUT SYMBOLS :\t\t";
		for (int i = 0; i < input_size; i++) cout << i << ", ";
		cout << "\n";

		cout << "\n START STATE :\t\t\t";
		for (int i = 0; i < start_state.size(); i++) cout << (char)(start_state[i] + 'A');
		cout << "\n";

		cout << "\n FINAL STATES :\t\t\t";
		for (int i = 0; i < final_states.size(); i++)
		{
			for (int j = 0; j < final_states[i].size(); j++) cout << (char)(final_states[i][j] + 'A');
			cout << ", ";
		}
		cout << "\n";

		cout << "\n\n CONVERT DFA TRANSITION TABLE \n\n";
		cout << "STATES\t";
		for (int i = 0; i < input_size; i++) cout << "| " << i << "\t";
		cout << "\n--------+------------------------------------\n";
		for (int i = 0; i < states.size(); i++)
		{
			for (int j = 0; j < states[i].size(); j++) cout << (char)(states[i][j] + 'A');
			for (int j = 0; j < input_size; j++)
			{
				cout << "\t| ";
				if (mapping_function.find({states[i], j}) == mapping_function.end())
				{
					cout << "-";
					continue;
				}
				State next_state = mapping_function[{ states[i], j }];
				for (int k = 0; k < next_state.size(); k++) cout << (char)(next_state[k] + 'A');
			}
			cout << "\n";
		}
		cout << "\n";
	}

};

class DFA
{
public:
	int state_size, input_size, start_state;
	State final_states;
	vector<vector<int>> mapping_function;
	bool isopt = false;

	DFA() {};

	DFA(ConvertDFA cdfa)
	{
		state_size = cdfa.states.size();
		input_size = cdfa.input_size;
		start_state = 0;

		for (int i = 0; i < cdfa.final_states.size(); i++)
			final_states.push_back(cdfa.state_idx[cdfa.final_states[i]]);

		for (int i = 0; i < state_size; i++)
		{
			mapping_function.push_back(vector<int>(input_size, 0));

			State curr_state = cdfa.states[i];
			for (int j = 0; j < input_size; j++)
			{
				if (cdfa.mapping_function.find({ curr_state, j }) == cdfa.mapping_function.end())
					mapping_function[cdfa.state_idx[curr_state]][j] = (int)('-' - 'A');
				else
				{
					State next_state = cdfa.mapping_function[{ curr_state, j }];
					mapping_function[cdfa.state_idx[curr_state]][j] = cdfa.state_idx[next_state];
				}
			}
		}
	}

	bool Partitioning(map<int, int> &state_to_group_idx, int groups_size, vector<int> &group)
	{
		bool ret = false;
		vector<int> result(input_size, 0), change_state;
		for (int i = 0; i < input_size; i++)
			result[i] = state_to_group_idx[mapping_function[group[0]][i]];
			
		for (auto state : group)
			for (int input = 0; input < input_size; input++)
			{
				if (result[input] != state_to_group_idx[mapping_function[state][input]])
				{
					change_state.push_back(state);
					break;
				}
			}

		if (change_state.size() != 0) ret = true;
		for (int i = 0; i < change_state.size(); i++)
			state_to_group_idx[change_state[i]] = groups_size;
		return ret;
	}

	DFA StateOptimization()
	{
		DFA mdfa;
		if (final_states.size() == state_size)
		{
			mdfa.state_size = 1;
			mdfa.input_size = input_size;
			mdfa.start_state = 0;
			mdfa.final_states.push_back(0);
			mdfa.mapping_function.resize(1, vector<int>(input_size, 0));
			for (int i = 0; i < input_size; i++) mdfa.mapping_function[0][i] = 0;
			mdfa.isopt = true;
			return mdfa;
		}

		int groups_size = 2;
		vector<vector<int>> groups(groups_size, vector<int>()), prev_groups;
		vector<int> check(state_size, 0);
		map<int, int> state_to_group_idx;
		for (int i = 0; i < final_states.size(); i++) check[final_states[i]] = 1;
		for (int i = 0; i < check.size(); i++)
		{
			groups[check[i]].push_back(i);
			state_to_group_idx.insert({ i, check[i] });
		}
		state_to_group_idx.insert({ -20, -20 });

		while (groups != prev_groups)
		{
			prev_groups = groups;
			for (auto group : groups)
			{
				if (Partitioning(state_to_group_idx, groups_size, group))
				{
					groups_size++;
					groups.clear();
					groups.resize(groups_size, vector<int>());
					for (int state = 0; state < state_size; state++)
						groups[state_to_group_idx[state]].push_back(state);
					break;
				}
			}
		}

		mdfa.state_size = groups_size;
		mdfa.input_size = input_size;
		mdfa.start_state = state_to_group_idx[start_state];

		check.resize(groups_size, 0);
		for (int i = 0; i < final_states.size(); i++)
			check[state_to_group_idx[final_states[i]]] = 1;
		for (int i = 0; i < check.size(); i++)
			if (check[i] == 1) mdfa.final_states.push_back(i);

		mdfa.mapping_function.resize(groups_size, vector<int>(input_size, 0));
		for (int state = 0; state < state_size; state++)
		{
			int curr_state = state_to_group_idx[state];
			for (int input = 0; input < input_size; input++)
				mdfa.mapping_function[curr_state][input] = state_to_group_idx[mapping_function[state][input]];
		}
		mdfa.isopt = true;
		return mdfa;
	}

	void PrintDFA()
	{
		cout << "\n\n";
		
		if (!isopt)
		{
			cout << "------------------------+---+------------------------\n";
			cout << "######################## DFA ########################\n";
			cout << "------------------------+---+------------------------\n";
		}
		else
		{
			cout << "----------------+-------------------+---------------\n";
			cout << "################ OPTIMIZATION OF DFA ################\n";
			cout << "----------------+-------------------+---------------\n";
		}
		cout << "\n";

		cout << "\n STATES OF NFA :\t\t";
		for (int i = 0; i < state_size; i++) cout << (char)(i + 'A') << ", ";
		cout << "\n";

		cout << "\n INPUT SYMBOLS :\t\t";
		for (int i = 0; i < input_size; i++) cout << i << ", ";
		cout << "\n";

		cout << "\n START STATE: \t\t\t" << (char)(start_state + 'A') << "\n";

		cout << "\n FINAL STATES: \t\t\t";
		for (int i = 0; i < final_states.size(); i++) cout << (char)(final_states[i] + 'A') << ", ";
		cout << "\n";

		cout << "\n\n DFA TRANSITION TABLE \n\n";
		cout << "STATES\t";
		for (int i = 0; i < input_size; i++) cout << "| " << i << "\t";
		cout << "\n";
		cout << "--------+------------------------------------\n";
		for (int i = 0; i < state_size; i++)
		{
			cout << (char)(i + 'A');
			for (int j = 0; j < input_size; j++)
				cout << "\t| " << (char)(mapping_function[i][j] + 'A');
			cout << "\n";
		}
		cout << "\n";
	}

};

class NFA
{
public:
	int state_size;
	int input_size;
	int start_state;
	vector<vector<string>> mapping_function;
	State final_states;
	vector<State> e_closures;

	NFA() {};
	NFA(int state_size, int input_size, int start_state, vector<vector<string>> mapping_function, State final_states)
	{
		this->state_size = state_size;
		this->input_size = input_size;
		this->start_state = start_state;
		this->mapping_function = mapping_function;
		this->final_states = final_states;
	}

	void GetAllClosure()
	{
		for (int i = 0; i < state_size; i++)
		{
			State closures;
			queue<int> que;
			vector<bool> visit(state_size, false);

			que.push(i);
			visit[i] = true;
			while (!que.empty())
			{
				int curr = que.front();
				que.pop();
				closures.push_back(curr);

				string next = mapping_function[curr][input_size];
				if (next == "-") continue;
				for (int j = 0; j < next.size(); j++)
				{
					int next_idx = next[j] - 'A';
					if (visit[next_idx]) continue;
					que.push(next_idx);
					visit[next_idx] = true;
				}
			}
			sort(closures.begin(), closures.end());
			e_closures.push_back(closures);
		}
	}

	State GetClosure(int idx)
	{
		return e_closures[idx];
	}

	State GetClosure(vector<int> states)
	{
		State next_state;
		vector<int> check(state_size, 0);
		for (int i = 0; i < states.size(); i++)
		{
			vector<int> closure = GetClosure(states[i]);
			for (int j = 0; j < closure.size(); j++) check[closure[j]] = 1;
		}
		for (int i = 0; i < check.size(); i++)
			if (check[i] == 1) next_state.push_back(i);
		return next_state;
	}

	State Move(int state, int input)
	{
		State next_states;
		string next = mapping_function[state][input];

		if (next == "-") return next_states;
		for (int i = 0; i < next.size(); i++)
			next_states.push_back((int)(next[i] - 'A'));
		return next_states;

	}

	State Move(vector<int> states, int input)
	{
		State next_state;
		vector<int> check(state_size, 0);
		for (int i = 0; i < states.size(); i++)
		{
			vector<int> temp = Move(states[i], input);
			for (int j = 0; j < temp.size(); j++) check[temp[j]] = 1;
		}
		for (int i = 0; i < check.size(); i++)
			if (check[i] == 1) next_state.push_back(i);
		return next_state;
	}

	State Dtrans(vector<int> state, int input)
	{
		return GetClosure(Move(state, input));
	}

	ConvertDFA MakeDFA()
	{
		ConvertDFA dfa(input_size);
		queue<State> que;
		set<State> check;
		int d_state_idx = 0;

		State start_Dstate = GetClosure(start_state);
		dfa.start_state = start_Dstate;
		que.push(start_Dstate);
		check.insert(start_Dstate);
		dfa.states.push_back(start_Dstate);
		dfa.FindFinalState(start_Dstate, final_states);
		dfa.state_idx.insert({ start_Dstate, d_state_idx++ });

		while (!que.empty())
		{
			State curr_state = que.front();
			que.pop();
			for (int i = 0; i < input_size; i++)
			{
				State new_state = Dtrans(curr_state, i);
				if (new_state.empty()) continue;

				dfa.mapping_function.insert({ { curr_state, i }, new_state });
				if (check.find(new_state) != check.end()) continue;
				que.push(new_state);
				check.insert(new_state);
				dfa.states.push_back(new_state);
				dfa.FindFinalState(new_state, final_states);
				dfa.state_idx.insert({ new_state, d_state_idx++ });
			}
		}
		return dfa;
	}

	void PrintNFA()
	{
		cout << "\n\n";
		cout << "------------------------+---+------------------------\n";
		cout << "######################## NFA ########################\n";
		cout << "------------------------+---+------------------------\n";
		cout << "\n";

		cout << "\n STATES OF NFA :\t\t";
		for (int i = 0; i < state_size; i++) cout << (char)(i + 'A') << ", ";
		cout << "\n";

		cout << "\n INPUT SYMBOLS :\t\t";
		for (int i = 0; i < input_size; i++) cout << i << ", ";
		cout << "eps\n";

		cout << "\n START STATE: \t\t\t" << (char)(start_state + 'A') << "\n";

		cout << "\n FINAL STATES: \t\t\t";
		for (int i = 0; i < final_states.size(); i++) cout << (char)(final_states[i] + 'A') << ", ";
		cout << "\n";

		cout << "\n\n NFA TRANSITION TABLE \n\n";
		cout << "STATES\t| ";
		for (int i = 0; i < input_size; i++) cout << i << "\t| ";
		cout << "eps\n";
		cout << "--------+------------------------------------\n";
		for (int i = 0; i < state_size; i++)
		{
			cout << (char)(i + 'A');
			for (int j = 0; j <= input_size; j++)
				cout << "\t| " << mapping_function[i][j];
			cout << "\n";
		}
	}

	void PrintClosure()
	{
		for (int i = 0; i < state_size; i++)
		{
			cout << "\n e-closure(" << (char)(i + 'A') << ") :\t\t";
			for (int j = 0; j < e_closures[i].size(); j++) cout << (char)(e_closures[i][j] + 'A');
			cout << "\n";
		}
	}
};

int main()
{
	while (true)
	{
		int state_size, input_size, final_states_size;
		char start_state, temp;
		vector<vector<string>> mapping_function;
		State final_states;

		cin >> state_size >> input_size >> start_state >> final_states_size;
		for (int i = 0; i < final_states_size; i++)
		{
			cin >> temp;
			final_states.push_back((int)(temp - 'A'));
		}

		for (int i = 0; i < state_size; i++)
		{
			string temp;
			mapping_function.push_back(vector<string>());
			for (int j = 0; j <= input_size; j++)
			{
				cin >> temp;
				mapping_function[i].push_back(temp);
			}
		}


		NFA nfa(state_size, input_size, (int)(start_state - 'A'), mapping_function, final_states);
		nfa.PrintNFA();
		nfa.GetAllClosure();
		nfa.PrintClosure();

		ConvertDFA cdfa = nfa.MakeDFA();
		cdfa.PrintConvertDFA();

		DFA dfa(cdfa);
		dfa.PrintDFA();

		DFA mdfa = dfa.StateOptimization();
		mdfa.PrintDFA();
	}
}