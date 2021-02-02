using namespace main_savitch_11;
// set():
//
template <class Item>
set<Item>::set() {
    data_count = 0;
    child_count = 0;
    for (auto& p : subset) {
        p = NULL;
    }
}

//
//
//

//set* b_tree_copy(const set* root_ptr):
//
template <class Item>
set<Item>* set<Item>::b_tree_copy(const set<Item>* root_ptr) {
    if (root_ptr == NULL) {
        return NULL;
    }
    set<Item>* set_ptr = new set<Item>;
    set_ptr->data_count = root_ptr->data_count;
    set_ptr->child_count = root_ptr->child_count;
    for (int i = 0; i < data_count; i++) {
        set_ptr->data[i] = root_ptr->data[i];
    }
    for(int i = 0; i < set_ptr->child_count; i++) {
        set_ptr->subset[i] = b_tree_copy(root_ptr->subset[i]);
    }
    return set_ptr;
}

//
//
//

// set(const set& source):
//
template <class Item>
set<Item>::set(const set<Item>& source) {
    data_count = source.data_count;
    child_count = source.child_count;
    for (int i = 0; i < data_count; i++) {
       data[i] = source.data[i];
    }
    this = b_tree_copy(&source);
}

//
//
//

//void clear( ):
//
template <class Item>
void set<Item>::clear() {
    for (auto& v : data) {
        v = Item();
    }
    for (auto& p : subset) {
        b_tree_clear(p);
    }
    data_count = 0;
    child_count = 0;
}

//
//
//

//void set<Item>::b_tree_clear(set<Item>*& root_ptr):
//
template <class Item>
void set<Item>::b_tree_clear(set<Item>*& root_ptr) {
    if (root_ptr != NULL) {
        for (auto& v : root_ptr->data) {
            v = Item();
        }
        for (int i = 0; i < root_ptr->child_count; i++) {
            b_tree_clear(root_ptr->subset[i]);
        }
        delete root_ptr;
        root_ptr = NULL;
    }
}

//
//
//

//void set<Item>::operator=(const set<Item>& source):
//
template <class Item>
void set<Item>::operator=(const set<Item>& source) {
    if (this == &source) {
        return;
    }
    clear();
    this = b_tree_copy(&source);
}

//
//
//

//std::size_t count(const Item& target) const
template <class Item>
std::size_t set<Item>::count(const Item& target) const {
    std::size_t i = get_index(target);
    if (i < data_count && !(target < data[i])) {
        return 1;
    }
    if (child_count == 0) {
        return 0;
    }
    return subset[i]->count(target);
}

//
//
//

//std::size_t set<Item>::get_index(const Item& entry):
template <class Item>
std::size_t set<Item>::get_index(const Item& entry) {
    for (std::size_t i = 0; i < data_count; i++) {
        if (!(data[i] < entry)) {
            return i;
        }
    }
    return data_count;
}

//
//
//

//bool set<Item>::loose_insert(const Item& entry)
template <class Item>
bool set<Item>::loose_insert(const Item& entry) {
    std::size_t i = get_index(entry);
        if (i < data_count && !(entry < data[i])) {
            return false;
        }
    if (child_count == 0) {
        for (std::size_t ix = data_count - 1; ix >= i; ix--) {
            data[ix + 1] = data[ix];
        }
       data[i] = entry;
       data_count++;
       return true;
    }
    bool b = subset[i]->loose_insert(entry);
    if (subset[i]->data_count == MAXIMUM + 1) {
        fix_excess(i);
    }
}

//
//
//

//void set<Item>::fix_excess(std::size_t i)
template <class Item>
void set<Item>::fix_excess(std::size_t i) {
   for (std::size_t ix = child_count - 1; ix > i; ix--) {
       subset[ix + 1] = subset[ix];
   }
   subset[i + 1] = new set<Item>;
   child_count++;
   for (std::size_t ix = MINIMUM + 1; ix < MAXIMUM + 2; ix++){
       subset[i + 1]->subset[ix - MINIMUM - 1] = subset[i]->subset[ix];
   }
   for (std::size_t ix = MINIMUM + 1; ix < MAXIMUM + 1; ix++){
       subset[i + 1]->data[ix - MINIMUM - 1] = subset[i] -> data[ix];
   }
   subset[i]->data_count = MINIMUM;
   subset[i + 1]->data_count = MINIMUM;
   subset[i]->child_count = MINIMUM + 1;
   subset[i + 1]->child_count = MINIMUM + 1;
   for (std::size_t ix = data_count - 1; ix >= i; ix--) {
       data[ix + 1] = data[ix];
   }
   data_count++;
   data[i] = subset[i]->data[MINIMUM];
}

//
//
//

//bool set<Item>::insert(const Item& entry):
template <class Item>
bool set<Item>::insert(const Item& entry) {
   if (!loose_insert(entry)) {
       return false;
   }
   if (data_count > MAXIMUM) {
       set<Item>* pset = new set<Item>;
       pset->subset[0] = this;
       pset->child_count = 1;
       this = pset;
       fix_excess(0);
   }
   return true;
}

//
//
//

//
template <class Item>
bool set<Item>::loose_erase(const Item& entry){

    size_t index = 0;
    while ((index < data_count) && (data[index] < entry)){ //if there is no index such that data[index] is not less than entry
        ++index; //then let index = data_count, indicating that all entries are less than target
      }
    bool found = (index < data_count) && !(entry < data[index]);

    if (is_leaf() && !found)
    	return false;
    if (is_leaf() && found) {
    	erase_entry(data, index);
    	--data_count;
    	return true;
    }
    if (!is_leaf() && !found) {
    	bool erased = subset[index]->loose_erase(entry);
    	if (subset[index]->data_count < MINIMUM)
    		fix_shortage(index);
    	//print();
    	return erased;
    }
    if (!is_leaf() && found) {
    	subset[index]->remove_biggest(data[index]);
    	if (subset[index]->data_count < MINIMUM)
    		fix_shortage(index);
    	return true;
    }
}

//
//
//

//
template <class Item>
void set<Item>::fix_shortage(std::size_t i) {
  if (within_subset(i - 1) && (subset[i - 1]->data_count > MINIMUM)) {
  	insert_entry(subset[i]->data, 0, data[i - 1]);
  	erase_entry(data, i - 1);
  	--data_count;  //update SET's data_count
  	++(subset[i]->data_count);  //update smaller SET's data_count

  	insert_entry(data, i - 1, subset[i - 1]->data.back());
  	++data_count;
  	subset[i - 1]->data.pop_back();
  	--(subset[i - 1]->data_count);  //update smaller SET's data_count

  	if (!(subset[i - 1]->is_leaf())) {
  		insert_entry(subset[i]->subset, 0, subset[i - 1]->subset.back());
  		++(subset[i]->child_count);
  		subset[i - 1]->subset.pop_back();
  		--(subset[i - 1]->child_count);
  	}
  }

  else if (within_subset(i + 1) && (subset[i + 1]->data_count > MINIMUM)) {

  	//move the original content of data[data_count-1] to become the last element of subset[index]->data.
  	//edit: move the original content of data[index] to become the last element of subset[index]->data.

  	//then make subset[index +1]->data[0] replace data[data_count-1].
  	//edit: then make subset[index +1]->data[0] replace data[index].

  	//then move subset[index + 1]->subset[0] from subset[index + 1]->subset to subset[index]->subset[child_count]

  	//subset[index]->data.push_back(data.back());
  	subset[i]->data.push_back(data[i]);
  	++(subset[i]->data_count);
  	//data.pop_back();
  	erase_entry(data, i);
  	--data_count;

  	//data.push_back(subset[index + 1]->data[0]);
  	insert_entry(data, i, subset[i + 1]->data[0]);
  	++data_count;
  	erase_entry(subset[i + 1]->data, 0);
  	--(subset[i + 1]->data_count);

  	if (!(subset[i + 1]->is_leaf())) {
  		subset[i]->subset.push_back(subset[i + 1]->subset[0]);
  		++(subset[i]->child_count);
  		erase_entry(subset[i + 1]->subset, 0);
  		--(subset[i + 1]->child_count);
  	}
  }

  else if (within_subset(i - 1) && (subset[i - 1]->data_count == MINIMUM)) {
  	subset[i - 1]->data.push_back(data[i - 1]);
  	++(subset[i - 1]->data_count);
  	erase_entry(data, i - 1);
  	--data_count;

  	//transfer items and children from subset[index] to the end of subset[index - 1]
  	(subset[i - 1]->data) += (subset[i]->data);
  	(subset[i - 1]->data_count) += (subset[i]->data_count);
  	(subset[i - 1]->subset) += (subset[i]->subset);
  	(subset[i - 1]->child_count) += (subset[i]->child_count);

  	//delete subset[index]
  	subset[i]->subset.clear();
  	subset[i]->child_count = 0;
  	delete subset[i];
  	erase_entry(subset, i);
  	--child_count;
  }

  else if (within_subset(i + 1) && (subset[i + 1]->data_count == MINIMUM)) {
  	subset[i]->data.push_back(data[i]);
  	++(subset[i]->data_count);
  	erase_entry(data, i);
  	--data_count;

  	//transfer items and children from subset[index + 1] to the end of subset[index]
  	(subset[i]->data) += (subset[ + 1]->data);
  	(subset[i]->data_count) += (subset[i + 1]->data_count);
  	(subset[i]->subset) += (subset[i + 1]->subset);
  	(subset[i]->child_count) += (subset[i + 1]->child_count);

  	//delete subset[index + 1]
  	subset[i + 1]->subset.clear(); //
  	subset[i + 1]->child_count = 0; //
  	delete subset[i + 1];
  	erase_entry(subset, i + 1);
  	--child_count;
  }
}
