
#include <iostream>
#include <vector>
#include <map>


using namespace std;

int global_depth, capacity;
int initial_global_depth;

struct bucket
{
    int local_depth = 0;
    vector<int> a;
};

map<int, int> dirc;         // hash_value ->bucket number
map<int, bucket *> storage; // bucket_number -> bucket_struct(localdepth,vector)

int hash_function(int value)
{
    int out = value % (1 << global_depth);
    return out;
}

void display()
{
    cout << "Global depth=" << max(global_depth, initial_global_depth) << endl;
    cout << "Directory" << endl;
    for (auto x : dirc)
    {
        cout << x.first << "=>" << x.second << endl;
    }
    cout << "Buckets" << endl;
    for (auto x : storage)
    {
        cout << "Bucket : " << x.first << endl;
        cout << "Local depth of the bucket is : " << x.second->local_depth << endl;
        cout << "Occupancy of the bucket is : " << (x.second->a).size() << endl;
        cout << "Stored value in the bucket is : ";
        sort((x.second->a).begin(), (x.second->a).end());
        for (auto y : (x.second->a))
        {
            cout << y << " ";
        }
        cout << endl;
    }
}

void split(int hash_value, bucket *specfic_bucket)
{
    bucket *newB = new bucket;
    specfic_bucket->a.clear();

    if (specfic_bucket->local_depth == global_depth)
    {

        if (global_depth < 20)
        {
            for (int i = (1 << global_depth); i < 2 * (1 << global_depth); i++)
            {
                dirc.insert(make_pair(i, (dirc[i % (1 << global_depth)])));
            }
            global_depth++;
        }
        else
        {
            cout << "Directory cann't expand now" << endl;
            display();
            return;
        }
    }

    int new_bucket_number = storage.size(); // number of newly formed bucket
    storage.insert(make_pair(new_bucket_number, newB));

    dirc[hash_value + (1 << (global_depth - 1))] = new_bucket_number;
    (specfic_bucket->local_depth)++;
    (storage[new_bucket_number]->local_depth) = specfic_bucket->local_depth;
    for (int i = (1 << global_depth); i < (1 << initial_global_depth); i++)
    {
        dirc[i] = dirc[hash_function(i)];
    }
}

void insert(int value)
{
    int hash_value = hash_function(value);
    int bucket_number = dirc[hash_value];

    if ((storage[bucket_number]->a).size() < capacity)
    {
        (storage[bucket_number]->a).push_back(value);
    }
    else
    {
        vector<int> temp;
        for (auto i : storage[bucket_number]->a)
        {
            temp.push_back(i);
        }
        split(hash_value, storage[bucket_number]);
        for (auto x : temp)
        {
            insert(x);
        }
        insert(value);
    }
}

bool search(int value)
{
    int hash_value = hash_function(value);
    int bucket_number = dirc[hash_value];
    bool found = false;

    for (auto x : (storage[bucket_number]->a))
    {
        if (x == value)
        {
            found = true;
        }
    }
    return found;
}

void merge(int hash_value)
{
    int hash_value_2;
    int bucket_number_1;
    int bucket_number_2;
    if (hash_value < (1 << global_depth - 1))
    {
        hash_value_2 = hash_value + (1 << global_depth - 1);
        bucket_number_1 = dirc[hash_value];
        bucket_number_2 = dirc[hash_value_2];
    }
    else
    {
        hash_value_2 = hash_value + (1 << global_depth - 1);
        bucket_number_1 = dirc[hash_value_2];
        bucket_number_2 = dirc[hash_value];
    }

    if (storage[bucket_number_1]->a.size() + storage[bucket_number_2]->a.size() <= capacity)
    {
        // Merging possible
        vector<int> temp;
        if (storage[bucket_number_2]->a.size() != 0)
        {
            for (auto x : storage[bucket_number_2]->a)
            {
                temp.push_back(x);
            }
        }
        for (int i = 0; i < dirc.size(); i++)
        {
            if (dirc[i] == bucket_number_2)
            {
                dirc[i] = bucket_number_1;
            }
            if (dirc[i] > bucket_number_2)
            {
                dirc[i]--;
            }
        }

        (storage[bucket_number_1]->local_depth) = (storage[bucket_number_1]->local_depth) - 1;

        for (int i = bucket_number_2; i < storage.size() - 1; i++)
        {
            storage[i] = storage[i + 1];
        }
        storage.erase(storage.size() - 1);
        if (temp.size() != 0)
        {
            for (auto x : temp)
            {
                insert(x);
            }
        }
    }
    display();
}

bool del(int value)
{
    int hash_value = hash_function(value);
    int bucket_number = dirc[hash_value];
    if (search(value))
    {
        for (auto i = (storage[bucket_number]->a).begin(); i != (storage[bucket_number]->a).end(); i++)
        {
            if (*i == value)
            {
                (storage[bucket_number]->a).erase(i);
                break;
            }
        }
        merge(hash_value);
        return true;
    }
    else
    {
        return false;
    }
    display();
}

int main()
{
    cin >> initial_global_depth >> capacity;
    for (int i = 0; i < (1 << initial_global_depth); i++)
    {
        dirc.insert(make_pair(i, 0));
    }
    global_depth = 0;
    bucket *B1 = new bucket;
    B1->local_depth = 0;
    storage.insert(make_pair(0, B1));

    int choice, temp;
    while (1)
    {
        cin >> choice;
        switch (choice)
        {
        case 1:
            cin >> temp;
            insert(temp);
            break;
        case 2:
            cin >> temp;
            if (del(temp))
            {
                cout << "Value is deleted from hash buckets" << endl;
            }
            else
            {
                cout << "Value don't present in hash buckets" << endl;
            }
            break;
        case 3:
            display();
            break;
        case 4:
            cin >> temp;
            if (search(temp))
            {
                cout << "Value is present in hash buckets" << endl;
            }
            else
            {
                cout << "Value don't present in hash buckets" << endl;
            }
            break;
        case 5:
            exit(0);
            break;
        default:
            cout << "Invalid choice !" << endl;
        }
    }
}

// cout << "1. Insertion\n";
// cout << "2. Deletion\n";
// cout << "3. Display\n";
// cout << "4. Search\n";
// cout << "5. Exit\n";

// Insert -> insert
// Delete -> del
// Search -> search
// Display -> display
// Split -> split
// Merge and bucket remover -> merge
