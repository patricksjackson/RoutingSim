#ifndef INPUTBUFFER_H
#define INPUTBUFFER_H

class InputBuffer : public Buffer
{
	public:
		InputBuffer ();
		InputBuffer ( size_t entries );
		~InputBuffer ();

		void WriteBack ( OutputBuffer* write_back );
		void PopPacket ( );
		void RoutePacket ( Address routerAddr );
		Direction GetRoute ( );
		size_t Routed ( );

	protected:

	private:
		Direction* routes;   //!< Buffer holding the routes of all the packets
		OutputBuffer* obuf;  //!< Paired Output Buffer for writeback information
		size_t buf_route;    //!< Pointer to next buffer space that needs to be routed

};

#endif

