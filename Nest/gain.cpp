#include <bits/stdc++.h>
#include <random>
#include <fstream>
#include <thread>
#include <unistd.h>
#include <fcntl.h>

// Global file stream
std::ofstream data_file;

using namespace std;

const int n = 1e4;
const int lim_m = 1e6;
const int lap_1 = 4500;
const int lap_2 = 5500;
const int step_1 = 200;
const int step_2 = 50;

const long long tot_edge = 1e8;

void init_file() {
    // data_file.open("data.csv", std::ios::app);  // append mode
    
    // if (!data_file.is_open()) {
    //     std::cerr << "Error opening file!\n";
    //     std::exit(1);
    // }

    // 1. Open the file using the open() syscall.
    // O_WRONLY: Write only
    // O_CREAT: Create the file if it doesn't exist
    // O_APPEND: Append to the end of the file
    // 0644: Standard file permissions (read/write for owner, read for others)
    int fd = open("1e4_.csv", O_WRONLY | O_CREAT | O_APPEND, 0644);
    
    if (fd == -1) {
        std::cerr << "Error: open() syscall failed!\n";
        std::exit(1);
    }

    // 2. Duplicate the file descriptor directly to Standard Output (stdout = 1)
    if (dup2(fd, STDOUT_FILENO) == -1) {
        std::cerr << "Error: dup2() failed to redirect stdout!\n";
        close(fd);
        std::exit(1);
    }

    // 3. Close the original file descriptor because stdout is now using it
    close(fd);
}

void record_data(std::ofstream &fout, int iter, int edge_count, int &cc_size, int &cc_count) {
    fout << iter << "," << n << "," << edge_count << "," << cc_size << "," << cc_count << "\n";
}


void close_file() {
    if (data_file.is_open()) {
        data_file.close();
    }
}

// 1. Make the RNG static so it only seeds ONCE for the entire program
static std::mt19937 rng(std::random_device{}());

pair<int, int> random_edge_generator(unordered_set<long long>& m, mt19937& rng) {
    std::uniform_int_distribution<long long> dist(0, tot_edge - 1);

    long long edge; // Declared once
    int a, b;

    while (true) {
        // 2. Remove "long long" here to use the outer variable!
        edge = dist(rng); 
        
        if (m.count(edge)) continue;
        
        a = edge / n; 
        b = edge % n;
        
        if (m.count(b * 1LL * n + a)) continue;
        if (a == b) continue;
        
        break;
    }

    // 3. Now 'edge' correctly holds the random value
    m.insert(edge);
    m.insert(b * 1LL * n + a);
    
    return make_pair(min(a, b), max(a, b));
}

int head(int a, vector<int> &par){
    if(a == par[a])return a;
    return par[a] = head(par[a], par);
}

void per_run(int iter, ofstream &fout){
    unordered_set<long long> mp;
    mp.reserve(lim_m * 2);

    // Use thread_local to prevent threads from fighting over the same RNG state
    static thread_local std::mt19937 thread_rng(std::random_device{}());
    
    vector <int> par(n), siz(n, 1);
    for(int i = 0; i < n; i++)par[i] = i;

    int max_cc_size = 1;
    int cc_count = n;
    pair<int, int> p;
    for(int i=1; i < lap_1; i += 1){
        p = random_edge_generator(mp, thread_rng);
        int a = head(p.first, par); int b = head(p.second, par);
        if(a != b){
            if(siz[a] < siz[b])swap(a, b);
            par[b] = par[a];
            siz[a] += siz[b];
            cc_count--; max_cc_size = max(max_cc_size, siz[a]);
        }
        if(i%step_1 == 0){
            record_data(fout, iter, i, max_cc_size, cc_count);
            if(cc_count == 1)return;
        }
    }

    for(int i=lap_1; i < lap_2; i += 1){
        p = random_edge_generator(mp, thread_rng);
        int a = head(p.first, par); int b = head(p.second, par);
        if(a != b){
            if(siz[a] < siz[b])swap(a, b);
            par[b] = par[a];
            siz[a] += siz[b];
            cc_count--; max_cc_size = max(max_cc_size, siz[a]);
        }
        if(i%step_2 == 0){
            record_data(fout, iter, i, max_cc_size, cc_count);
            if(cc_count == 1)return;
        }
    }

    for(int i=lap_2; i <= lim_m; i += 1){
        p = random_edge_generator(mp, thread_rng);
        int a = head(p.first, par); int b = head(p.second, par);
        if(a != b){
            if(siz[a] < siz[b])swap(a, b);
            par[b] = par[a];
            siz[a] += siz[b];
            cc_count--; max_cc_size = max(max_cc_size, siz[a]);
        }
        if(i%step_1 == 0){
            record_data(fout, iter, i, max_cc_size, cc_count);
            if(cc_count == 1)return;
        }
    }
}

void thread_worker(int thread_id, int num_iterations) {
    string filename = "data_thread_" + to_string(thread_id) + ".csv";
    ofstream fout(filename);
    
    for(int i = 1; i <= num_iterations; i++) {
        per_run(i, fout);
    }
    
    fout.close();
}

int main(){
    // init_file();
    // cout<<"Gettings tarted!"<<endl;
    // for(int i=1; i<=10000; i++){
    //     per_run(i);
    // }

    // close_file();
    // return 0;

    int total_simulations = 10000;
    int num_threads = 4;
    int sims_per_thread = total_simulations / num_threads;

    vector<thread> workers;

    for(int i = 0; i < num_threads; i++) {
        // Launch a thread calling thread_worker
        workers.push_back(thread(thread_worker, i, sims_per_thread));
    }

    // Wait for all 4 threads to finish
    for(auto &t : workers) {
        t.join();
    }

    cout << "All simulations complete!" << endl;
    return 0;
}