#include "watcher.h"

#include <thread>
#include <system.h>
#include <file.h>

Watcher::~Watcher() {}
Watcher::Watcher() : mWatchShader(false) {}

void Watcher::cleanup() {
	mWatchShader = false;
	mShaderWatcher.join();
}

void Watcher::addShaderFile( const string& name, std::function<void()>&& callback ) {
	mShaderCallback[name].push_back( callback );
}

void Watcher::watchShaderDir() {
	mWatchShader = true;
	mShaderWatcher = std::thread( watchingShaderDir, &mWatchShader, &mShaderCallback );
}


void Watcher::watchingShaderDir( const bool* isWatching, CallbackMap* shaderCallback ) {
#ifdef _WIN32
	HANDLE file = CreateFileA( File::GetShaderRootDir().c_str(),
							   FILE_LIST_DIRECTORY, FILE_SHARE_READ, NULL, OPEN_EXISTING,
							   FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL );

	ASSERT_FALSE( (file != INVALID_HANDLE_VALUE), "Invalid shader directory" );

	OVERLAPPED overlapped;
	overlapped.hEvent = CreateEvent( NULL, FALSE, 0, NULL );

	u8 changeBuf[1024];
	BOOL success = ReadDirectoryChangesW( file, changeBuf, 1024, TRUE,
										  FILE_NOTIFY_CHANGE_LAST_WRITE,
										  NULL, &overlapped, NULL );

	while (*isWatching) {
		if (!overlapped.hEvent)
			continue;
		
		DWORD result = WaitForSingleObject( overlapped.hEvent, 0 );
		if (result != WAIT_OBJECT_0)
			continue;
			
		DWORD bytesTransferred;
		GetOverlappedResult( file, &overlapped, &bytesTransferred, FALSE );
		FILE_NOTIFY_INFORMATION* event = (FILE_NOTIFY_INFORMATION*)changeBuf;

		for (;;) {
			wstring wsname( event->FileName, event->FileNameLength / sizeof( wchar_t ) );
			string name( wsname.begin(), wsname.end() );

			LOG( name + " updated");
			for (const auto& callback : (*shaderCallback)[name]) {
				if (callback == nullptr)
					continue;

				try {
					callback();
				} catch (const std::exception& e) {
					WAR( e.what() );
				}
			}

			if (event->NextEntryOffset) {
				*((uint8_t**)&event) += event->NextEntryOffset;
			} else {
				break;
			}
		}
		BOOL success = ReadDirectoryChangesW( file, changeBuf, 1024, TRUE,
											  FILE_NOTIFY_CHANGE_LAST_WRITE,
											  NULL, &overlapped, NULL );
		
	}
#endif
}
