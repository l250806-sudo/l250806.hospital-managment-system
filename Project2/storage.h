#ifndef STORAGE_H
#define STORAGE_H

// Generic template storage class - holds up to 100 objects in a static array
// No std::vector allowed per assignment rules
template <typename T>
class Storage
{
private:
    T   data[100]; // fixed-size array, no vector
    int count;     // how many objects are currently stored

public:
    Storage()
    {
        count = 0;
    }

    // add a new object - returns false if full
    bool add(const T& obj)
    {
        if (count >= 100)
            return false;
        data[count] = obj; // uses T's assignment operator (must be defined)
        count++;
        return true;
    }

    // remove object by ID - shifts remaining elements left
    bool removeByID(int id)
    {
        for (int i = 0; i < count; i++)
        {
            if (data[i].getId() == id)
            {
                // shift everything after this one position left
                for (int j = i; j < count - 1; j++)
                    data[j] = data[j + 1];
                count--;
                return true;
            }
        }
        return false; // not found
    }

    // find and return pointer to object by ID, nullptr if not found
    T* findByID(int id)
    {
        for (int i = 0; i < count; i++)
        {
            if (data[i].getId() == id)
                return &data[i];
        }
        return nullptr;
    }

    // returns pointer to the internal array (for iteration)
    T* getAll()
    {
        return data;
    }

    // how many objects are stored
    int size() const
    {
        return count;
    }

    // clear everything
    void clear()
    {
        count = 0;
    }
};

#endif