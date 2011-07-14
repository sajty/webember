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

#ifndef WEBEMBERMESSAGEQUEUE_H
#define WEBEMBERMESSAGEQUEUE_H

#include "WebEmber.h"
#include "BrowserHost.h"
/**
 * @brief Provides a global message queue, to receive messages from ember.
 * 
 * @see http://www.boost.org/doc/libs/1_46_1/doc/html/interprocess/synchronization_mechanisms.html#interprocess.synchronization_mechanisms.message_queue
 * 
 * The Message Queue name is "WEBEMBER_PLUGIN", you can send messages to this from anywhere.
 * - The messages are text based.
 * - Max message size is equal to WebEmberMessageQueue::mMQBuffSize.
 * - The first word of each message is the command.
 * - The command can contain upper-case letters and the '_' character.
 */
class WebEmberMessageQueue{
public:
	WebEmberMessageQueue(WebEmberWeakPtr plugin);

	/**
	 * @brief Runs the queue with the calling thread. The function will quit, when it receives a "QUIT" message.
	 */
	int run();

	/**
	 * @brief Sends "QUIT" to the message queue. The MQ thread will exit, after this call.
	 */
	void stop();

	/**
	 * @brief This should be the entry point for a separated MQ thread.
	 */
	static int messageQueueThread(boost::shared_ptr<WebEmberMessageQueue> queue);

protected:
	/**
	 * @brief This will be called after every received message. Contains message handling code.
	 * 
	 * @returns if the function returns true, the queue should be shut down.
	 */
	inline bool processMessage(char* msg, unsigned int msg_size, unsigned int msg_priority);
	
	/**
	 * @brief This is a wrapper to evaluate JavaScript with main thread,because its not thread safe.
	 * 
	 * It should be called like this:
	 * browser->ScheduleOnMainThread(browser, boost::bind(&WebEmberMessageQueue::evaluateJavaScript, browser, code));
	 * The browser object needs to be passed, which will be weak_ptr, until the main thread has time to execute this call.
	 * If the weak_ptr is invalid on the calling time, the function will not be called, so this function will not guarantee,
	 * that the javascript is executed or not.
	 */
	static void evaluateJavaScript(FB::BrowserHostPtr browser, std::string code);

	/**
	 * @brief The max length of the messages. 
	 */

	static const std::size_t mMQBuffSize = 1024;

	/**
	 * With this weak_ptr, if the plugin's main thread and the plugins objects are shut down, MessageQueue will work.
	 */
	WebEmberWeakPtr mPlugin;
};

#endif // WEBEMBERMESSAGEQUEUE_H