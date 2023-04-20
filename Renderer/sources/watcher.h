#pragma once

#include <common.h>
#include <map>

class Watcher {

public:

	~Watcher();
	Watcher();

	void cleanup();

	void addShaderFile( const string& name, std::function<void()>&& callback );

	void watchShaderDir();


private:
	typedef vector<std::function<void()>> CallbackVector;
	typedef std::map<string, CallbackVector> CallbackMap;

	Cleaner mCleaner;
	CallbackMap mShaderCallback;

	std::thread mShaderWatcher;
	bool mWatchShader;
	static void watchingShaderDir( const bool* isWatching, CallbackMap* shaderCallback );

};

