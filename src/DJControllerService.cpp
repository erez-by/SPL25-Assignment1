#include "DJControllerService.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>

DJControllerService::DJControllerService(size_t cache_size)
    : cache(cache_size) {}
/**
 * TODO: Implement loadTrackToCache method
 */
int DJControllerService::loadTrackToCache(AudioTrack& track) {
    std::string trackName = track.get_title();
    if(cache.contains(trackName)) {
        cache.get(trackName);
        return 1;
    }

    PointerWrapper<AudioTrack> trackClone = track.clone();
    if(!trackClone) {
        std::cout <<"[ERROR] null pionter , faild to load"<< trackName << std::endl;
        return 0;
    }

    trackClone -> load();
    trackClone -> analyze_beatgrid();
    bool didEvect = cache.put(std::move(trackClone));
    if(didEvect) return -1;
    return 0;
}

void DJControllerService::set_cache_size(size_t new_size) {
    cache.set_capacity(new_size);
}
//implemented
void DJControllerService::displayCacheStatus() const {
    std::cout << "\n=== Cache Status ===\n";
    cache.displayStatus();
    std::cout << "====================\n";
}

/**
 * TODO: Implement getTrackFromCache method
 */
AudioTrack* DJControllerService::getTrackFromCache(const std::string& track_title) {
    AudioTrack* track = cache.get(track_title);
    if(!track)std::cout <<"[ERROR] track:"<< track_title <<"faild to clone"<< std::endl; 
    return track;
}
