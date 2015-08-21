#ifndef EVENTFRAGMENTRECORD_H_
#define EVENTFRAGMENTRECORD_H_

#include <iostream>
#include <vector>
#include <stdio.h>

#include "FormattedRecord.h"


/* ---------------------------------------------------------------------- */

class TriggerHeader {
public:
	struct THdata{
		unsigned int unknown1[2] ;
		unsigned long long counter ;
		unsigned long long trgtime ;
		unsigned long long deadtime ;
		unsigned int unknown2[1];
	};
	
	TriggerHeader(unsigned int *buffer=NULL){
    if (buffer) {
			for (int i=0; i<9; i++) ((unsigned int *)&m_th)[i] = buffer[i];
			unsigned long long counter = getCounter();
			m_tdc = -1;
			for (int j=0;j<64;j++){
				if(((counter>>j)&1)==0){
					m_tdc=j;
					break;
				}
			}
		} 
	}

	TriggerHeader(std::vector<unsigned int> *buffer=NULL, unsigned int index=0){
    if (buffer) {
			for (int i=0; i<9; i++) ((unsigned int *)&m_th)[i] = buffer->at(index + i);
			unsigned long long counter = getCounter();
			m_tdc = -1;
			for (int j=0;j<64;j++){
				if(((counter>>j)&1)==0){
					m_tdc=j;
					break;
				}
			}
		} 
	}
	
	unsigned long long getCounter() {return m_th.counter;}
	unsigned long long getTrgTime() {
	  unsigned long long helper;
		helper = m_th.trgtime & 0xFFFF;
		if (helper < 3) helper = m_th.trgtime + 0x10000;
		else helper = m_th.trgtime;
	  return helper;
	}
	unsigned long long getDeadTime() {return m_th.deadtime;}      
	int getTDC() {return m_tdc;} 
	
private:
	THdata m_th;
	int m_tdc;
};


class EventFragmentRecord{
public:    
    // This constructor should receive a size of a structure and a pointer.
    EventFragmentRecord(unsigned int size=4, unsigned int *buffer=NULL) {
			m_size = size;
			if (buffer) {
				if (size < 11*sizeof(unsigned int)) std::cout << "This will fail" << std::endl;
				m_eventNum = buffer[1];
				m_triggerHead = new TriggerHeader(&buffer[2]);
				m_data = new std::vector<unsigned int>;
				for (int i=0; i<(size/sizeof(unsigned int)-11); i++) m_data->push_back(buffer[i+11]);
			}
    }

    EventFragmentRecord(std::vector<unsigned int> *buffer=NULL, unsigned int index=0) {
		if (buffer) {	
			if (index < buffer->size()) {
				m_size = buffer->at(index++);
				if (buffer) {
					if (m_size < 11*sizeof(unsigned int)) std::cout << "This will fail" << std::endl;
					m_eventNum = buffer->at(index++);
					m_triggerHead = new TriggerHeader(buffer, index);
					index+=9;
					m_data = new std::vector<unsigned int>;
					m_data->clear();
					for (int i=0; i<(m_size/sizeof(unsigned int)-11); i++) m_data->push_back(buffer->at(index+i));
				}
			}
		}
    }

    EventFragmentRecord(std::istream *fs) {
			fs->read((char*)&m_size, 4);
			unsigned int *buffer = new unsigned int[m_size/4 - 1];
			fs->read((char *)&buffer[0], m_size);
			if (buffer) {
				if (m_size < 11*sizeof(unsigned int)) std::cout << "This will fail" << std::endl;
				m_eventNum = buffer[1];
				m_triggerHead = new TriggerHeader(&buffer[2]);
				m_data = new std::vector<unsigned int>;
				for (int i=0; i<(m_size/sizeof(unsigned int)-11); i++) m_data->push_back(buffer[i+11]);
			}
    }
			

    
    ~EventFragmentRecord() {
		delete m_data;
		delete m_triggerHead;
    }
	
    bool isValid(){return ((m_size >= 11*sizeof(unsigned int)) && (m_size%4==0));}
    int nHits(){
			unsigned long hitCount = 0;
			for (int i=0; i < m_data->size(); i++) if (getRecord(i).isData()) hitCount++;
			return hitCount;
    }
    int size() {return m_data->size();}
	int length() {return m_size/4;}
    
	unsigned int getEventNum() {return m_eventNum;}
    unsigned long long getTriggerID() {return m_triggerHead->getTrgTime();}
	std::string getTriggerTime(time_t timeStamp) {
				std::string timeString;
				char timeText[100];
  			struct tm * timeinfo;
				time_t now = timeStamp + ((int) ((double)getTriggerID() * 25.6e-9));	
  			timeinfo = localtime ( &now );
				strftime(timeText, 100, "%c", timeinfo);
				timeString = timeText;
				return timeString;
    }
    unsigned long long getCounter() {return m_triggerHead->getCounter();}
    unsigned long long getDeadTime() {return m_triggerHead->getDeadTime();}
    int getTDC() {return m_triggerHead->getTDC();}
	
    FormattedRecord getRecord(int i) {
		return FormattedRecord(m_data->at(i));
    }
	
private:
    unsigned int m_size;
    unsigned int m_eventNum;
    TriggerHeader *m_triggerHead;
    std::vector<unsigned> *m_data;
};

#endif
