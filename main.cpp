
#include <iostream>
#include <vector>
#include <iomanip>
#include <queue>
#include <algorithm>
#include <climits>
#include <string>
using namespace std;

struct Timeline {
    int process_id;
    int start_time;
    int end_time;
};

void printGanttChart(const vector<Timeline>& timeline, int total_time)
{
    if (timeline.empty()) return;
    
    cout << "\nGantt Chart:\n";
    vector<Timeline> consolidated;
    for (const auto& t : timeline)
    {
        if (!consolidated.empty() && consolidated.back().process_id == t.process_id)
        {

            consolidated.back().end_time = t.end_time;
        }
        else
        {

            consolidated.push_back(t);
        }
    }

    string process_line = "| ";
    for (const auto& t : consolidated)
    {
        if (t.process_id == 0)
            process_line += "Idle | ";
        else
            process_line += "P" + to_string(t.process_id) + " | ";
    }
    cout << process_line << "\n";

    string time_line(process_line.length(), ' ');

    vector<int> pipe_positions;
    for (int i = 0; i < process_line.length(); ++i)
    {
        if (process_line[i] == '|')
            pipe_positions.push_back(i);
    }

    for (int i = 0; i < pipe_positions.size(); ++i)
    {
        string time_str = to_string(i == 0 ? 0 : consolidated[i - 1].end_time);
        int pipe_pos = pipe_positions[i];

        int time_len = time_str.length();
        int center_pos = pipe_pos - time_len / 2;

        for (int j = 0; j < time_len; ++j)
        {
            int pos = center_pos + j;
            if (pos >= 0 && pos < time_line.length())
                time_line[pos] = time_str[j];
        }
    }
    
    cout << time_line << "\n";
}

struct QueueState {
    int time;
    vector<int> queue;
    int cpu_process;
};

void printReadyQueueTimeline(const vector<QueueState>& states, int total_time)
{
    if (states.empty()) return;
    
    cout << "\nReady Queue Timeline (Compact View):\n";
    for (const auto& state : states)
    {
        cout << "Time " << state.time << ": [";
        for (int i = 0; i < state.queue.size(); ++i)
        {
            if (i > 0) cout << ", ";
            cout << "P" << state.queue[i];
        }
        cout << "] -> ";
        if (state.cpu_process > 0)
            cout << "CPU: P" << state.cpu_process;
        else
            cout << "Done";
        cout << "\n";
    }
}

vector<Timeline> fcfs_findavgTime(const vector<int>& bt, const vector<int>& at)
{
    int n = (int)bt.size();
    vector<int> wt(n, 0), tat(n, 0);
    vector<Timeline> timeline;

    vector<int> idx(n);
    for (int i = 0; i < n; ++i) idx[i] = i;
    stable_sort(idx.begin(), idx.end(), [&](int a, int b){
        if (at[a] != at[b]) return at[a] < at[b];
        return a < b;
    });

    int current_time = 0;
    for (int k = 0; k < n; ++k)
    {
        int i = idx[k];
        if (current_time < at[i]) {

            timeline.push_back({0, current_time, at[i]});
            current_time = at[i];
        }
        int start = current_time;
        wt[i] = start - at[i];
        tat[i] = wt[i] + bt[i];
        timeline.push_back({i+1, start, current_time + bt[i]});
        current_time += bt[i];
    }

    int total_wt = 0, total_tat = 0;
    cout << setw(8) << "Process" << setw(10) << "Arrival" << setw(8) << "Burst" << setw(12) << "Waiting" << setw(14) << "Turnaround" << '\n';
    for (int i = 0; i < n; ++i)
    {
        total_wt += wt[i];
        total_tat += tat[i];
        cout << setw(8) << i+1 << setw(10) << at[i] << setw(8) << bt[i] << setw(12) << wt[i] << setw(14) << tat[i] << '\n';
    }
    cout << fixed << setprecision(2);
    cout << "Average waiting time = " << (double)total_wt / n << '\n';
    cout << "Average turnaround time = " << (double)total_tat / n << '\n';
    
    return timeline;
}

vector<int> getQueueContents(queue<int> q) {
    vector<int> result;
    while (!q.empty()) {
        result.push_back(q.front() + 1);
        q.pop();
    }
    return result;
}

vector<Timeline> rr_findavgTime(const vector<int>& bt, const vector<int>& at, int quantum)
{
    int n = (int)bt.size();
    vector<int> rem_bt = bt;
    vector<int> wt(n, 0), tat(n, 0);
    vector<bool> finished(n, false), in_queue(n, false);
    vector<Timeline> timeline;
    vector<QueueState> queue_states;

    queue<int> q;
    int t = 0;
    int completed = 0;

    for (int i = 0; i < n; ++i)
    {
        if (at[i] <= t && !in_queue[i]) { q.push(i); in_queue[i] = true; }
    }

    if (q.empty())
    {
        int next_at = INT_MAX;
        for (int i = 0; i < n; ++i) if (!finished[i]) next_at = min(next_at, at[i]);
        timeline.push_back({0, t, next_at});
        t = next_at;
        for (int i = 0; i < n; ++i) if (at[i] <= t && !in_queue[i]) { q.push(i); in_queue[i] = true; }
    }

    while (completed < n)
    {
        if (q.empty())
        {
            int next_at = INT_MAX;
            for (int i = 0; i < n; ++i) if (!finished[i] && !in_queue[i]) next_at = min(next_at, at[i]);
            timeline.push_back({0, t, next_at});
            t = max(t, next_at);
            for (int i = 0; i < n; ++i) if (at[i] <= t && !finished[i] && !in_queue[i]) { q.push(i); in_queue[i] = true; }
            continue;
        }

        int front_proc = q.front();
        auto curr_q = getQueueContents(q);
        if (!curr_q.empty()) {
            curr_q.erase(curr_q.begin());
        }
        queue_states.push_back({t, curr_q, front_proc + 1});

        int i = q.front(); q.pop(); in_queue[i] = false;
        int exec = min(quantum, rem_bt[i]);
        int start = t;

        for (int unit = 0; unit < exec; ++unit)
        {
            rem_bt[i]--;
            t++;

            for (int j = 0; j < n; ++j)
            {
                if (!finished[j] && !in_queue[j] && at[j] == t)
                {
                    q.push(j);
                    in_queue[j] = true;
                }
            }
        }
        
        timeline.push_back({i+1, start, t});

        if (rem_bt[i] == 0)
        {
            finished[i] = true;
            completed++;
            tat[i] = t - at[i];
            wt[i] = tat[i] - bt[i];
        }
        else
        {
            q.push(i);
            in_queue[i] = true;
        }
    }

    queue_states.push_back({t, {}, 0});

    int total_wt = 0, total_tat = 0;
    cout << setw(8) << "Process" << setw(10) << "Arrival" << setw(8) << "Burst" << setw(12) << "Waiting" << setw(14) << "Turnaround" << '\n';
    for (int i = 0; i < n; ++i)
    {
        total_wt += wt[i];
        total_tat += tat[i];
        cout << setw(8) << i+1 << setw(10) << at[i] << setw(8) << bt[i] << setw(12) << wt[i] << setw(14) << tat[i] << '\n';
    }
    cout << fixed << setprecision(2);
    cout << "Average waiting time = " << (double)total_wt / n << '\n';
    cout << "Average turnaround time = " << (double)total_tat / n << '\n';
    
    printReadyQueueTimeline(queue_states, timeline.empty() ? 0 : timeline.back().end_time);
    
    return timeline;
}

vector<Timeline> spn_findavgTime(const vector<int>& bt, const vector<int>& at)
{
    int n = (int)bt.size();
    vector<int> wt(n, 0), tat(n, 0);
    vector<bool> completed(n, false);
    vector<Timeline> timeline;
    
    int current_time = 0;
    int processes_done = 0;
    
    while (processes_done < n)
    {
        int best = -1;
        int min_burst = INT_MAX;

        for (int i = 0; i < n; ++i)
        {
            if (!completed[i] && at[i] <= current_time && bt[i] < min_burst)
            {
                best = i;
                min_burst = bt[i];
            }
        }

        if (best == -1)
        {
            int next_at = INT_MAX;
            for (int i = 0; i < n; ++i)
            {
                if (!completed[i]) next_at = min(next_at, at[i]);
            }
            timeline.push_back({0, current_time, next_at});
            current_time = next_at;
            continue;
        }
        
        int start = current_time;
        wt[best] = start - at[best];
        tat[best] = wt[best] + bt[best];
        timeline.push_back({best+1, start, current_time + bt[best]});
        current_time += bt[best];
        completed[best] = true;
        processes_done++;
    }
    
    int total_wt = 0, total_tat = 0;
    cout << setw(8) << "Process" << setw(10) << "Arrival" << setw(8) << "Burst" << setw(12) << "Waiting" << setw(14) << "Turnaround" << '\n';
    for (int i = 0; i < n; ++i)
    {
        total_wt += wt[i];
        total_tat += tat[i];
        cout << setw(8) << i+1 << setw(10) << at[i] << setw(8) << bt[i] << setw(12) << wt[i] << setw(14) << tat[i] << '\n';
    }
    cout << fixed << setprecision(2);
    cout << "Average waiting time = " << (double)total_wt / n << '\n';
    cout << "Average turnaround time = " << (double)total_tat / n << '\n';
    
    return timeline;
}

vector<Timeline> srt_findavgTime(const vector<int>& bt, const vector<int>& at)
{
    int n = (int)bt.size();
    vector<int> rem_bt = bt;
    vector<int> wt(n, 0), tat(n, 0);
    vector<bool> completed(n, false);
    vector<Timeline> timeline;
    vector<QueueState> queue_states;
    
    int current_time = 0;
    int processes_done = 0;
    
    while (processes_done < n)
    {
        int best = -1;
        int min_remaining = INT_MAX;

        for (int i = 0; i < n; ++i)
        {
            if (!completed[i] && at[i] <= current_time && rem_bt[i] < min_remaining)
            {
                best = i;
                min_remaining = rem_bt[i];
            }
        }
        if (best == -1)
        {
            int next_at = INT_MAX;
            for (int i = 0; i < n; ++i)
            {
                if (!completed[i]) next_at = min(next_at, at[i]);
            }
            timeline.push_back({0, current_time, next_at});
            current_time = next_at;
            continue;
        }
        vector<int> ready_queue;
        for (int i = 0; i < n; ++i)
        {
            if (i != best && !completed[i] && at[i] <= current_time && rem_bt[i] > 0)
            {
                ready_queue.push_back(i + 1);
            }
        }
        queue_states.push_back({current_time, ready_queue, best + 1});
        int start = current_time;
        rem_bt[best]--;
        current_time++;
        timeline.push_back({best+1, start, current_time});
        
        if (rem_bt[best] == 0)
        {
            tat[best] = current_time - at[best];
            wt[best] = tat[best] - bt[best];
            completed[best] = true;
            processes_done++;
        }
    }

    queue_states.push_back({current_time, {}, 0});
    
    int total_wt = 0, total_tat = 0;
    cout << setw(8) << "Process" << setw(10) << "Arrival" << setw(8) << "Burst" << setw(12) << "Waiting" << setw(14) << "Turnaround" << '\n';
    for (int i = 0; i < n; ++i)
    {
        total_wt += wt[i];
        total_tat += tat[i];
        cout << setw(8) << i+1 << setw(10) << at[i] << setw(8) << bt[i] << setw(12) << wt[i] << setw(14) << tat[i] << '\n';
    }
    cout << fixed << setprecision(2);
    cout << "Average waiting time = " << (double)total_wt / n << '\n';
    cout << "Average turnaround time = " << (double)total_tat / n << '\n';
    
    printReadyQueueTimeline(queue_states, timeline.empty() ? 0 : timeline.back().end_time);
    
    return timeline;
}

int main()
{
    int n;
    cout << "Enter number of processes: ";
    cin >> n;
    if (n <= 0) return 0;
    
    vector<int> burst(n), arrival(n);
    for (int i = 0; i < n; ++i)
    {
        cout << "Enter burst time for process " << i+1 << ": ";
        cin >> burst[i];
        if (burst[i] < 0) burst[i] = 0;
    }
    for (int i = 0; i < n; ++i)
    {
        cout << "Enter arrival time for process " << i+1 << ": ";
        cin >> arrival[i];
        if (arrival[i] < 0) arrival[i] = 0;
    }
    
    int quantum;
    cout << "Enter time quantum for Round Robin: ";
    cin >> quantum;
    if (quantum <= 0) quantum = 1;

    cout << "\n========== FCFS Scheduling ==========\n";
    auto fcfs_timeline = fcfs_findavgTime(burst, arrival);
    int fcfs_total = (fcfs_timeline.empty() ? 0 : fcfs_timeline.back().end_time);
    printGanttChart(fcfs_timeline, fcfs_total);

    cout << "\n========== Round Robin Scheduling (quantum=" << quantum << ") ==========\n";
    auto rr_timeline = rr_findavgTime(burst, arrival, quantum);
    int rr_total = (rr_timeline.empty() ? 0 : rr_timeline.back().end_time);
    printGanttChart(rr_timeline, rr_total);

    cout << "\n========== SPN (Shortest Process Next) ==========\n";
    auto spn_timeline = spn_findavgTime(burst, arrival);
    int spn_total = (spn_timeline.empty() ? 0 : spn_timeline.back().end_time);
    printGanttChart(spn_timeline, spn_total);

    cout << "\n========== SRT (Shortest Remaining Time) ==========\n";
    auto srt_timeline = srt_findavgTime(burst, arrival);
    int srt_total = (srt_timeline.empty() ? 0 : srt_timeline.back().end_time);
    printGanttChart(srt_timeline, srt_total);

    return 0;
}
