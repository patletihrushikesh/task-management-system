


#include<bits/stdc++.h>
using namespace std;

// okay so... this is like a quick & dirty Task Scheduler
// emergency tasks first obviously, then by priority, duration, id
// might clean up later (if it even works lol)

struct TaskRec {
    int ID;
    int dur;
    int pr;
    int emerg;
    int start=-1;
    int endt=-1;
    int turn=-1;
    int wait=-1;

    TaskRec(int i=0, int d=0, int p=0, int e=0)
        : ID(i), dur(d), pr(p), emerg(e) {}
};

// was gonna use priority_queue but nah I wanted to "see" the heap
struct Node {
    int idx;
};

class WeirdHeap {
    vector<Node> h;                 // lol should rename to heap but too lazy
    const vector<TaskRec>* ref = nullptr;

    bool higher(const Node &a, const Node &b) const {
        const auto &A = (*ref)[a.idx];
        const auto &B = (*ref)[b.idx];
        if (A.emerg != B.emerg) return A.emerg > B.emerg;
        if (A.pr != B.pr) return A.pr > B.pr;
        if (A.dur != B.dur) return A.dur < B.dur;
        return A.ID < B.ID;
    }

    void siftUp(int i) {
        while (i > 0) {
            int parent = (i - 1) / 2;
            if (higher(h[i], h[parent])) {
                swap(h[i], h[parent]);
                i = parent;
            } else break;
        }
    }

    void siftDown(int i) {
        int n = h.size();
        while (1) {
            int L = 2*i + 1, R = L + 1, best = i;
            if (L < n && higher(h[L], h[best])) best = L;
            if (R < n && higher(h[R], h[best])) best = R;
            if (best == i) break;
            swap(h[i], h[best]);
            i = best;
        }
    }

public:
    WeirdHeap(const vector<TaskRec>* t=nullptr) { ref = t; }
    void setRef(const vector<TaskRec>* t) { ref = t; }
    bool empty() const { return h.empty(); }
    int size() const { return (int)h.size(); }

    void push(int idx) {
        h.push_back({idx});
        siftUp(h.size() - 1);
    }

    int pop() {
        if (h.empty()) return -1;
        int top = h[0].idx;
        h[0] = h.back();
        h.pop_back();
        if (!h.empty()) siftDown(0);
        return top;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cout << "Enter number of tasks: ";
    cin >> n;

    if (n <= 0) {
        cerr << "hmm invalid count\n";
        return 0;
    }

    vector<TaskRec> arr;
    arr.reserve(n);

    cout << "Enter each task (id dur pr emerg[0/1]):\n";

    for (int i = 0; i < n; i++) {
        int id, d, p, e;
        cin >> id >> d >> p >> e;
        if (d <= 0) {
            cerr << "duration messed up\n";
            return 0;
        }
        if (e != 0 && e != 1) {
            cerr << "emergency must be 0/1\n";
            return 0;
        }
        arr.emplace_back(id, d, p, e);
    }

    WeirdHeap heap;
    heap.setRef(&arr);
    for (int i = 0; i < n; i++) heap.push(i);

    int time = 0;
    vector<int> doneOrder;

    while (!heap.empty()) {
        int idx = heap.pop();
        auto &t = arr[idx];

        t.start = time;
        // could have added arrival time but skipped
        t.endt = time + t.dur;
        t.turn = t.endt;   // since arrival is 0
        t.wait = t.start;  // same logic again
        time = t.endt;

        doneOrder.push_back(idx);

        // debug (commented out):
        // cout << "Scheduled: " << t.ID << " -> " << t.start << "-" << t.endt << endl;
    }

    cout << "\n== FINAL SCHEDULE ==\n";
    cout << "Seq\tID\tEmerg\tPr\tStart\tEnd\tDur\tTurn\tWait\n";
    for (int i = 0; i < (int)doneOrder.size(); i++) {
        auto &T = arr[doneOrder[i]];
        cout << i+1 << "\t" << T.ID << "\t" << T.emerg << "\t"
             << T.pr << "\t" << T.start << "\t" << T.endt << "\t"
             << T.dur << "\t" << T.turn << "\t" << T.wait << "\n";
    }

    // metrics block (rough)
    int makespan = time;
    long long waitSum = 0;
    int waited = 0;

    for (auto &t : arr) {
        waitSum += t.wait;
        if (t.wait > 0) waited++;
    }

    double avgW = (arr.size() ? (double)waitSum / arr.size() : 0.0);

    cout << "\nTotal time: " << makespan;
    cout << "\nAvg waiting: " << fixed << setprecision(2) << avgW;
    cout << "\n#Tasks that waited: " << waited << "\n";

    // random check (why not)
    if (makespan < 0) cerr << "??? Negative makespan??? weird\n";

    // TODO: maybe print Gantt chart (later)
    // Also, maybe sort by start time before printing? hmm.

    return 0;
}