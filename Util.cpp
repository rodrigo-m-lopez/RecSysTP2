#include "Util.h"
#include "Logger.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream> 
#include <algorithm>
#include <unordered_set>


content * read_content(std::string path)
{
    content * input = new content;

    input->length = 0;
    input->insert_index = 0;
    
    std::string line;
    std::ifstream myfile(path);

    Logger::Log("Counting lines");

    while (getline(myfile, line))
            input->length++;
    input->length--; //remove header from count
    int length = input->length;
    std::string text = std::to_string(length);
    text += " lines";
    Logger::Log(text);

    input->data = new content_node[input->length];

    std::ifstream infile(path);

    std::string header;
    getline(infile, header);//ignore header

    Logger::Log("Loading content to memory");
    
    while (infile >> *input){}

    Logger::Log("Content loaded...");

    return input;
}

std::istream & operator>>(std::istream & stream, content & input)
{
    //input format:
//    i0012278,{"Title":"The 'High Sign'","Year":"1921","Rated":"N/A","Released":"18 Apr 1921",
//            "Runtime":"21 min","Genre":"Short, Comedy","Director":"Edward F. Cline, Buster Keaton",
//            "Writer":"Edward F. Cline, Buster Keaton","Actors":"Buster Keaton",
//            "Plot":"Buster is thrown off a train near an amusement park. There he gets a job in a "
//    "shooting gallery run by the Blinking Buzzards mob. Ordered to kill a businessman, he winds up "
//    "protecting the man and his daughter by outfitting their home with trick devices.",
//            "Language":"English","Country":"USA","Awards":"N/A",
//            "Poster":"http://ia.media-imdb.com/images/M/MV5BMTc3ODY1MDg4Ml5BMl5BanBnXkFtZTcwMjYzMDE2MQ@@._V1_SX300.jpg",
//            "Metascore":"N/A","imdbRating":"7.7","imdbVotes":"1,502","imdbID":"tt0012278","Type":"movie","Response":"True"}
    std::string line;
    std::string garbage;
    std::string itemId, title, year, rated, released, runtime, genre, director, writer, actors, plot, language, country, awards;
    std::string poster, metascore, imdb_rating, imdb_votes, imdb_id, type, response;
    if (std::getline(stream, line))
    {
        std::stringstream iss(line);

        if (std::getline(iss, itemId, ',') && std::getline(iss, garbage, ':') && std::getline(iss, garbage, '"') &&
            std::getline(iss, title, '"') && std::getline(iss, garbage, ':') && std::getline(iss, garbage, '"') &&
            std::getline(iss, year, '"') && std::getline(iss, garbage, ':') && std::getline(iss, garbage, '"') &&
            std::getline(iss, rated, '"') && std::getline(iss, garbage, ':') && std::getline(iss, garbage, '"') &&
            std::getline(iss, released, '"') && std::getline(iss, garbage, ':') && std::getline(iss, garbage, '"') &&
            std::getline(iss, runtime, '"') && std::getline(iss, garbage, ':') && std::getline(iss, garbage, '"') &&
            std::getline(iss, genre, '"') && std::getline(iss, garbage, ':') && std::getline(iss, garbage, '"') &&
            std::getline(iss, director, '"') && std::getline(iss, garbage, ':') && std::getline(iss, garbage, '"') &&
            std::getline(iss, writer, '"') && std::getline(iss, garbage, ':') && std::getline(iss, garbage, '"') &&
            std::getline(iss, actors, '"') && std::getline(iss, garbage, ':') && std::getline(iss, garbage, '"') &&
            std::getline(iss, plot, '"') && std::getline(iss, garbage, ':') && std::getline(iss, garbage, '"') &&
            std::getline(iss, language, '"') && std::getline(iss, garbage, ':') && std::getline(iss, garbage, '"') &&
            std::getline(iss, country, '"') && std::getline(iss, garbage, ':') && std::getline(iss, garbage, '"') &&
            std::getline(iss, awards, '"') && std::getline(iss, garbage, ':') && std::getline(iss, garbage, '"') &&
            std::getline(iss, poster, '"') && std::getline(iss, garbage, ':') && std::getline(iss, garbage, '"') &&
            std::getline(iss, metascore, '"') && std::getline(iss, garbage, ':') && std::getline(iss, garbage, '"') &&
            std::getline(iss, imdb_rating, '"') && std::getline(iss, garbage, ':') && std::getline(iss, garbage, '"') &&
            std::getline(iss, imdb_votes, '"') && std::getline(iss, garbage, ':') && std::getline(iss, garbage, '"') &&
            std::getline(iss, imdb_id, '"') && std::getline(iss, garbage, ':') && std::getline(iss, garbage, '"') &&
            std::getline(iss, type, '"') && std::getline(iss, garbage, ':') && std::getline(iss, garbage, '"') &&
            std::getline(iss, response, '"') && std::getline(iss, garbage, '\n'))
        {
            content_node * node = new content_node;
            node->item_id = itemId;
            node->title = title;
            node->year = tryConvertToInt(year);
            node->rated = rated;
            node->released = released;
            node->runtime_minutes = tryConvertToInt(runtime);
            node->genre = genre;
            node->director = director;
            node->writer = writer;
            node->actors = actors;
            node->plot = plot;
            node->language = language;
            node->country = country;
            node->awards = awards;
            node->poster = poster;
            node->metascore = metascore;
            node->imdbRating = tryConvertToFloat(imdb_rating);
            node->imdbVotes = tryConvertToInt(imdb_votes);
            node->imdbID = imdb_id;
            node->type = type;
            node->response = response == "True";
            
            input.updateTitleDictionary(node);
            input.updateDescriptionDictionary(node);
            input.data[input.insert_index] = *node;
            input.insert_index++;
        }
        else
        {
            Logger::Log("Error dos brabos");
            stream.setstate(std::ios::failbit);
        }        
    }
    return stream;
}

void content::updateDictionary(dictionary * dictionary_to_update, std::string text)
{
    std::stringstream string_stream(text);
    std::string word;
    while (string_stream.good())
    {
        string_stream >> word;
        dictionary_to_update->update(word);
    } 
}


void dictionary::update(std::string word)
{
    static std::unordered_set<std::string> * stopwords = getStopWords();
    
    std::transform(word.begin(), word.end(), word.begin(), ::tolower);
    
    word.erase(std::remove_if(word.begin(), word.end(),[](char c) { return !std::isalpha(c); } ), word.end());
    
    if(!stopwords->count(word))
    {
        (*data)[word]++;
    }
}

std::unordered_set<std::string> * getStopWords()
{
    std::unordered_set<std::string> * result = new std::unordered_set<std::string>;
    std::ifstream infile("StopWords");
    
    std::string stopWord;
    while(infile.good())
    {
        std::getline(infile, stopWord);
        result->insert(stopWord);
    }
    return result;
}

void content::updateTitleDictionary(content_node * node)
{
    updateDictionary(title_dictionary, node->title);
}
   
void content::updateDescriptionDictionary(content_node * node)
{
    updateDictionary(description_dictionary , node->actors);
    updateDictionary(description_dictionary , node->awards);
    updateDictionary(description_dictionary , node->country);
    updateDictionary(description_dictionary , node->director);
    updateDictionary(description_dictionary , node->genre);
    updateDictionary(description_dictionary , node->language);
    updateDictionary(description_dictionary , node->plot);
    updateDictionary(description_dictionary , node->rated);
    updateDictionary(description_dictionary , node->type);
    updateDictionary(description_dictionary , node->writer);
}
    
float tryConvertToFloat(std::string text)
{
    try 
    {
        return std::stof(text);
    } 
    catch(...) 
    {
        return 0.0f;
    }
}

int tryConvertToInt(std::string text)
{
    try 
    {
        return std::stoi(text);
    } 
    catch(...) 
    {
        return 0;
    }
}
