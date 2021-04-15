#pragma once

class iComponent {
public:
	virtual ~iComponent(void) { }

	// ID values for engine use
	const unsigned mFriendlyID;
	unsigned mParentID;

protected:
	iComponent(void) { }
};
