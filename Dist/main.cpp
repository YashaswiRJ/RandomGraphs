#include <bits/stdc++.h>
#include <random>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>

// Global file stream
std::ofstream data_file;

using namespace std;

const int n = 1e4;
const int lim_m = 1e6;

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
    int fd = open("distribution_1e4.csv", O_WRONLY | O_CREAT | O_APPEND, 0644);
    
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

void record_data(int iter, int edge_count) {
    cout << iter << "," << edge_count << "\n";
}


void close_file() {
    if (data_file.is_open()) {
        data_file.close();
    }
}

// 1. Make the RNG static so it only seeds ONCE for the entire program
static std::mt19937 rng(std::random_device{}());

pair<int, int> random_edge_generator(unordered_set<long long>& m) {
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

void per_run(int iter){
    unordered_set<long long> mp;
    mp.reserve(lim_m * 2);

    vector <int> par(n), siz(n, 1);
    for(int i = 0; i < n; i++)par[i] = i;

    int max_cc_size = 1;
    int cc_count = n;
    pair<int, int> p;
    for(int i=1; i < lim_m; i += 1){
        p = random_edge_generator(mp);
        int a = head(p.first, par); int b = head(p.second, par);
        if(a != b){
            if(siz[a] < siz[b])swap(a, b);
            par[b] = par[a];
            siz[a] += siz[b];
            cc_count--; max_cc_size = max(max_cc_size, siz[a]);
        }
        if(cc_count == 1){
            record_data(iter, i); return;
        }
    }

}

int main(){
    init_file();
    cout<<"Gettings tarted!"<<endl;
    for(int i=1; i<=10000; i++){
        per_run(i);
    }

    close_file();
    return 0;
}