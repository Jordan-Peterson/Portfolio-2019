package dhms.wireformats;

public interface Protocols {
	
	int CLIENT_SENDS_LOGIN = 0;
	int SERVER_RESPONDS_LOGIN = 1;
	
	int CLIENT_SENDS_CREATE_ACCOUNT = 2;
	int SERVER_RESPONDS_CREATE_ACCOUNT = 3;
	
	int CLIENT_SENDS_CHANGE_ACCOUNT_DETAILS = 4;
	int SERVER_RESPONDS_CHANGE_ACCOUNT_DETAILS = 5;
	
	int CLIENT_SENDS_GET_ACCOUNT_DETAILS = 6;
	int SERVER_RESPONDS_GET_ACCOUNT_DETAILS = 7;
	
	int CLIENT_SENDS_AVAILABLE_ROOMS = 8;
	int SERVER_RESPONDS_AVAILABLE_ROOMS = 9;
	
	int CLIENT_SENDS_BOOK_ROOM = 10;
	int SERVER_RESPONDS_BOOK_ROOM = 11;
	
	int CLIENT_SENDS_CANCEL_BOOK = 12;
	int SERVER_RESPONDS_CANCEL_BOOK = 13;
	
	int CLIENT_SENDS_GET_BOOKED_ROOMS = 14;
	int SERVER_RESPONDS_GET_BOOKED_ROOMS = 15;
	
	int CLIENT_SENDS_GET_ALL_EVENTS = 16;
	int SERVER_RESPONDS_GET_ALL_EVENTS = 17;
	
	int CLIENT_SENDS_ATTENDED_EVENT = 18;
	int SERVER_RESPONDS_ATTENDED_EVENT = 19;
	
	int CLIENT_SENDS_CANCEL_EVENT = 20;
	int SERVER_RESPONDS_CANCEL_EVENT = 21;
	
	int CLIENT_SENDS_GET_SCHEDULED_EVENTS = 22;
	int SERVER_RESPONDS_GET_SCHEDULED_EVENTS = 23;
	
	int CLIENT_SENDS_DELETE_ACCOUNT = 24;
	int SERVER_RESPONDS_DELETE_ACCOUNT = 25;
	
	int CLIENT_SENDS_LOGOUT = 26;
	int SERVER_RESPONDS_lOGOUT = 27;
	

}
