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

class DFA
{
public:
	vector<State> states;
	int input_size;
	vector<int> start_state, final_state;
	map<pair<State, int>, State> mapping_function;

	DFA() {};
	DFA(int input_size)
	{
		this->input_size = input_size;
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

	DFA MakeDFA()
	{
		DFA dfa(input_size);
		queue<vector<int>> que;
		set<State> check;

		State start_state = GetClosure(start_state);
		que.push(start_state);
		check.insert(start_state);
		dfa.states.push_back(start_state);

		while (!que.empty())
		{
			State curr_state = que.front();
			que.pop();
			for (int i = 0; i < input_size; i++)
			{
				State new_state = Dtrans(curr_state, i);
				if (check.find(new_state) == check.end()) continue;
				que.push(new_state);
				check.insert(new_state);
				dfa.states.push_back(new_state);
				dfa.mapping_function.insert({ { curr_state, i }, new_state });

			}

		}
	}

	void PrintNFA()
	{
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

		cout << "\n NFA mapping function tale \n\n\n";
		cout << "STATES\t|";
		for (int i = 0; i < input_size; i++) cout << i << "\t|";
		cout << "eps\n";
		cout << "--------+------------------------------------\n";
		for (int i = 0; i < state_size; i++)
		{
			cout << (char)(i + 'A');
			for (int j = 0; j <= input_size; j++)
				cout << "\t|" << mapping_function[i][j];
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


	State new_state = nfa.Dtrans(nfa.GetClosure(0), 1);
	for (int i = 0; i < new_state.size(); i++) cout << (char)(new_state[i] + 'A') << " ";
	cout << "\n";
}