#include "DJLibraryService.h"
#include "SessionFileParser.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>
#include <filesystem>


DJLibraryService::DJLibraryService(const Playlist& playlist) 
    : playlist(playlist), library() {}
/**
 * @brief Load a playlist from track indices referencing the library
 * @param library_tracks Vector of track info from config
 */
void DJLibraryService::buildLibrary(const std::vector<SessionConfig::TrackInfo>& library_tracks) {
    int count =0;
    for(SessionConfig::TrackInfo trackInfo : library_tracks){

        AudioTrack* newTrack = nullptr;

        if(trackInfo.type == "MP3"){
              newTrack = new MP3Track(
                trackInfo.title,trackInfo.artists,trackInfo.duration_seconds,
                trackInfo.bpm, 
               trackInfo.extra_param1, (bool)trackInfo.extra_param2);
            library.push_back(newTrack);
            std::cout<<"– MP3: MP3Track created:"<<trackInfo.title<<"at :"
            << trackInfo.extra_param1 <<"kbps" <<std::endl;
            count ++;

        }
        else if(trackInfo.type == "WAV"){
              newTrack = new WAVTrack(
                trackInfo.title,trackInfo.artists,trackInfo.duration_seconds,
                trackInfo.bpm, 
               trackInfo.extra_param1,trackInfo.extra_param2);
                library.push_back(newTrack);
                 std::cout<<"– WAV: WAVTrack created:"<<trackInfo.title<<"at :"
               << trackInfo.extra_param1 <<"HZ/"<<trackInfo.extra_param2 << "bit"<<std::endl;
            count ++;
        }      
    }
    std::cout <<"[INFO] Track library built:" <<count<<"tracks loaded"<<std::endl;  
}

/**
 * @brief Display the current state of the DJ library playlist
 * 
 */
void DJLibraryService::displayLibrary() const {
    std::cout << "=== DJ Library Playlist: " 
              << playlist.get_name() << " ===" << std::endl;

    if (playlist.is_empty()) {
        std::cout << "[INFO] Playlist is empty.\n";
        return;
    }

    // Let Playlist handle printing all track info
    playlist.display();

    std::cout << "Total duration: " << playlist.get_total_duration() << " seconds" << std::endl;
}

/**
 * @brief Get a reference to the current playlist
 * 
 * @return Playlist& 
 */
Playlist& DJLibraryService::getPlaylist() {
    // Your implementation here
    return playlist;
}

/**
 * TODO: Implement findTrack method
 * 
 * HINT: Leverage Playlist's find_track method
 */
AudioTrack* DJLibraryService::findTrack(const std::string& track_title) {
    return playlist.find_track(track_title);
}

void DJLibraryService::loadPlaylistFromIndices(const std::string& playlist_name, 
                                               const std::vector<int>& track_indices) {
    std::cout<<"[INFO] Loading playlist:"<< playlist_name << std::endl;
    Playlist newPlaylist(playlist_name)  ;
    for(int i: track_indices){
        if(i<0 || i>library.size()){
            std::cout<<"[WARNING] Invalid track index:"<< i << std::endl;
        }
        else{
            AudioTrack * sourceTrack = library[i-1];
            PointerWrapper<AudioTrack> warappedTrack = sourceTrack->clone();
            AudioTrack * newTrack = warappedTrack.release();

            if(!newTrack){
                std::cout<<"[ERROR] null track index:"<< i << std::endl;
            }
             newTrack->load();
             newTrack->analyze_beatgrid();
             newPlaylist.add_track(newTrack);
             std::cout<<"Added:"<< newTrack->get_title() << "to playlist"<< playlist_name << std::endl;   

        }
    }                                     
    std::cout<<"[INFO] Playlist loaded:"<< playlist_name << newPlaylist.get_track_count() <<"tracks"<< std::endl;

}
/**
 * TODO: Implement getTrackTitles method
 * @return Vector of track titles in the playlist
 */
std::vector<std::string> DJLibraryService::getTrackTitles() const {
    std::vector<std::string> TrackTitles;
    for(AudioTrack* track :playlist.getTracks()){
       auto title = track -> get_title();
        TrackTitles.push_back(title);
    }
    return TrackTitles;
}

DJLibraryService::~DJLibraryService() {
    for(AudioTrack * track : library){
        delete track;
    }
    
}