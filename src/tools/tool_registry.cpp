// src/tools/tool_registry.cpp — YOU IMPLEMENT (spec §4.3).
//
// A hand-written hash table. No std::unordered_map / std::map / std::set.
// Every function below is a stub so the starter compiles. Replace each body.

#include "tools/tool_registry.h"

ToolRegistry::ToolRegistry(){
    current_array_length = 16;
    arr_buckets = new std::vector<std::unique_ptr<Tool>>[current_array_length];
    current_num_items = 0; 
    maximum_probe_length = 0;

}  

ToolRegistry::~ToolRegistry(){
    delete [] arr_buckets;
};   // unique_ptrs in your buckets free the tools

std::size_t ToolRegistry::hash(std::string_view key) noexcept {
    
    // FNV-1a as provided in Appendix A
    std::uint64_t h = 1469598103934665603ULL;                         // "offset basis"
    for (unsigned char c : key) { h ^= c; h *= 1099511628211ULL; }     // "FNV prime"
    return static_cast<std::size_t>(h);
    
    return 0;
}

// std::vector<std::unique_ptr<Tool>> * arr_buckets;

bool ToolRegistry::add(std::unique_ptr<Tool> tool) {
    int index;
    
    // Can't add a tool that doesn't exist
    if(tool == nullptr){return false;}

    // Can't add a tool that already exists
    if(find(tool->name()) != nullptr){
        return false;
    }

    // If load factor is too high, or there is an absurdly long maximum probe length, rehash everything.
    if ((load_factor() > 0.8) || (max_probe_length() > 32)){
        rehash();
    }
    
    // Compute the to-be index via our hash function
    index = hash(tool->name()) % bucket_count();
    
    // Add it to the bucket at its index.
    arr_buckets[index].push_back(std::move(tool));

    // In case the bucket is now a grand winner in the maximum probe length race
    if(static_cast<double>(arr_buckets[index].size()) > maximum_probe_length){
        maximum_probe_length = arr_buckets[index].size();
    }

    current_num_items++;
    return true;
}

void ToolRegistry::rehash(){
    std::vector<std::unique_ptr<Tool>> * arr_buckets_new = new std::vector<std::unique_ptr<Tool>>[bucket_count() * 2];
    for (int i = 0; i < static_cast<int>(bucket_count()); i++){

        // Don't bother traversing an empty linked list.
        if(arr_buckets[i].empty()){ continue; }
        
        // Rehash each member of linked list into the new hash table
        while(!arr_buckets[i].empty()){
            
            // Kind of sus. No idea if this will work
            int index_new = hash(arr_buckets[i].back()->name()) % (bucket_count() * 2);
                
            // No idea if this will work either.
            arr_buckets_new[index_new].push_back(std::move(arr_buckets[i].back()));
            arr_buckets[i].pop_back();
                
            // In case the new bucket is now a grand winner in the maximum probe length race
            if(static_cast<int>(arr_buckets_new[index_new].size()) > maximum_probe_length){
                maximum_probe_length = arr_buckets_new[index_new].size();
            }
        }
    }

    // Replaces the old bucket array with the new one. Hopefully.
    delete [] arr_buckets;
    arr_buckets = arr_buckets_new;
    current_array_length *= 2;
};

Tool* ToolRegistry::find(std::string_view name) const {
    // Find the index based on the hash of the name
    int index = hash(name) % bucket_count();

    // Traverse the linked list there.

    if(arr_buckets[index].empty()){ return nullptr; }

    // Stackexchange and cppreference my beloved
    // Looks in the bucket for a match for the name 
    for(auto&& iter : arr_buckets[index]){

        if(iter->name() == name){
            return iter.get();
        }
    }

    // If it isn't found, return nullptr    
    return nullptr;   
}

std::size_t ToolRegistry::size() const noexcept {
    return current_num_items;   // Returns current number of items
}

void ToolRegistry::for_each(const std::function<void(const Tool&)>& fn) const {
    
    // Traverse every bucket
    for (int i = 0; i < static_cast<int>(bucket_count()); i++){
        // Skip empty buckets
        if(arr_buckets[i].empty()){continue;} 

        // Performs function fn() on each tool in the bucket
        for(auto&& curr : arr_buckets[i]){
            // Wary as to whether this will actually work
            fn(*curr.get());
        }
    }
}

std::size_t ToolRegistry::bucket_count() const noexcept {
    return current_array_length;   // Returns current number of buckets
}

double ToolRegistry::load_factor() const noexcept {
    return static_cast<double>(size()) / static_cast<double>(bucket_count());
}

std::size_t ToolRegistry::max_probe_length() const {
    return maximum_probe_length;   // Returns maximum probe length
}
