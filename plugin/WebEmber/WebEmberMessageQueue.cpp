/*
 Copyright (C) 2011 Peter Szucs

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include "WebEmberMessageQueue.h"
#include "logging.h"
#include <boost/thread/thread.hpp>
#include <boost/thread/locks.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <iostream>

WebEmberMessageQueue::WebEmberMessageQueue(WebEmberWeakPtr plugin) :
	mPlugin(plugin)
{
}
int WebEmberMessageQueue::messageQueueThread(boost::shared_ptr<WebEmberMessageQueue> queue)
{
	return queue->run();
}
int WebEmberMessageQueue::run()
{
	//ensure that the old queue is removed.
	boost::interprocess::message_queue::remove("WEBEMBER_PLUGIN");

	FBLOG_INFO("WebEmberMessageQueue::run", "Starting message queue.");
	try {
		boost::interprocess::message_queue mq(boost::interprocess::create_only, "WEBEMBER_PLUGIN", 32, mMQBuffSize);
		char buff[mMQBuffSize + 1];
		//protect the buffer from overflows.
		buff[mMQBuffSize] = 0;
		//will be the message length
		std::size_t ret_size;
		//will be the message priority
		unsigned int ret_priority;
		bool quit;
		do {
			mq.receive(buff, mMQBuffSize, ret_size, ret_priority);
			quit = processMessage(buff, ret_size, ret_priority);
		} while (!quit);

	} catch (std::exception ex) {
		FBLOG_ERROR("WebEmberMessageQueue::run", "Exception thowed on the message queue: " << ex.what());
	} catch (...) {
		FBLOG_ERROR("WebEmberMessageQueue::run", "Exception thowed on the message queue.");
	}
	FBLOG_INFO("WebEmberMessageQueue::run", "Message queue shut down.");

	boost::interprocess::message_queue::remove("WEBEMBER_PLUGIN");

	return 0;
}
void WebEmberMessageQueue::stop()
{
	FBLOG_INFO("WebEmberMessageQueue::stop", "Request message queue shut down.");
	try {
		boost::interprocess::message_queue mq(boost::interprocess::open_only, "WEBEMBER_PLUGIN");
		mq.send("QUIT", 5, 0);
	} catch (const std::exception& ex) {
		FBLOG_ERROR("WebEmberMessageQueue::stop", "Failed to send a message: " << ex.what());
	} catch (...) {
		FBLOG_ERROR("WebEmberMessageQueue::stop", "Failed to send a message.");
	}
}

inline bool WebEmberMessageQueue::processMessage(char* msg, std::size_t msg_size, unsigned int msg_priority)
{
	//the first word describes the message command.
	char* endofCommand = strchr(msg, ' ');

	if (!endofCommand) {
		endofCommand = msg + strlen(msg);
	}
	int commandLength = endofCommand - msg;
	if (commandLength < 1 || commandLength > 8) {
		FBLOG_ERROR("WebEmberMessageQueue::processMessage", "Received a message with incorrect command.");
		return false;
	}

	//command is a word with max 8 uppercase letters.
	std::string command = std::string(msg, commandLength);
	if (command.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ_") != std::string::npos) {
		FBLOG_ERROR("WebEmberMessageQueue::processMessage", "Received a message with incorrect characters in the command.");
		return false;
	}
	if (command == "RUN") {
		if (*endofCommand != '\0') {
			endofCommand++;
		}
		std::string str(endofCommand);
		WebEmberPtr plugin(mPlugin.lock());
		if (plugin) {
			FB::BrowserHostPtr browser = plugin->getBrowserHost();
			try {
				browser->ScheduleOnMainThread(browser, boost::bind(&WebEmberMessageQueue::evaluateJavaScript, browser, str));
			} catch (std::exception ex) {
				FBLOG_ERROR("WebEmberMessageQueue::processMessage", "Failed to run javascript. " << ex.what());
			} catch (...) {
				FBLOG_ERROR("WebEmberMessageQueue::processMessage", "Failed to run javascript.");
			}
		} else {
			FBLOG_ERROR("WebEmberMessageQueue::processMessage", "Failed to run javascript. WebEmberWeakPtr is returning null.");
		}
	} else if (command == "QUIT") {
		//will shut down message queue thread by returning true.
		return true;
	} else {
		FBLOG_ERROR("WebEmberMessageQueue::processMessage", "Received a message with unknown command: " << command);
	}
	return false;
}
void WebEmberMessageQueue::evaluateJavaScript(FB::BrowserHostPtr browser, std::string str)
{
	browser->evaluateJavaScript(str);
}
