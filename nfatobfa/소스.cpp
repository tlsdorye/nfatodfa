#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <algorithm>

using namespace std;

class NFA
{
public:
	int state_size;
	int input_size;
	int start_state;
	vector<vector<string>> mapping_function;
	vector<int> final_states;
	vector<vector<int>> e_closures;

	NFA() {};
	NFA(int state_size, int input_size, int start_state, vector<vector<string>> mapping_function, vector<int> final_states)
	{
		this->state_size = state_size;
		this->input_size = input_size;
		this->start_state = start_state;
		this->mapping_function = mapping_function;
		this->final_states = final_states;
	}

	void getClosures()
	{
		for (int i = 0; i < state_size; i++)
		{
			vector<int> closures;
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
	vector<int> final_states;

	cin >> state_size >> input_size >> start_state >> final_states_size;
	for (int i = 0; i < final_states_size; i++)
	{
		cin >> temp;
		final_states.push_back((int)(temp-'A'));
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

	
	NFA nfa(state_size, input_size, (int)(start_state-'A'),mapping_function, final_states);
	nfa.PrintNFA();
	nfa.getClosures();
	nfa.PrintClosure();
}