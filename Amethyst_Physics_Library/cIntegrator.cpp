#include "cIntegrator.hpp"

namespace phys
{
	cIntegrator::cIntegrator()	{

	}

	// EULER ALGORITHM
	void cIntegrator::Euler(glm::vec3& pos, glm::vec3& vel, glm::vec3& acc, float dt) {
		// TODO: math!
	}

	void cIntegrator::Euler(glm::vec3& pos, glm::vec3& vel, glm::vec3& acc, const glm::vec3& appliedAcc, float dt) {
		// TODO: math!
	}

	// EULER-CROMER ALGORITHM
	void cIntegrator::EulerCromer(glm::vec3& pos, glm::vec3& vel, glm::vec3& acc, float dt) {
		// TODO: math!
	}

	void cIntegrator::EulerCromer(glm::vec3& pos, glm::vec3& vel, glm::vec3& acc, const glm::vec3& appliedAcc, float dt) {
		// TODO: math!
	}

	// MIDPOINT ALGORITHM
	void cIntegrator::MidPoint(glm::vec3& pos, glm::vec3& vel, glm::vec3& acc, float dt) {
		// TODO: math!
	}

	void cIntegrator::MidPoint(glm::vec3& pos, glm::vec3& vel, glm::vec3& acc, const glm::vec3& appliedAcc, float dt) {
		// TODO: math!
	}

	// VELOCITY-VERLET ALGORITHM <- with half-step acceleration lag
	void cIntegrator::VelocityVerlet(glm::vec3& pos, glm::vec3& vel, glm::vec3& acc, float dt) {
		// TODO: math!
	}

	void cIntegrator::VelocityVerlet(glm::vec3& pos, glm::vec3& vel, glm::vec3& acc, const glm::vec3& appliedAcc, float dt)	{
		// TODO: math!
	}

	struct State {
		glm::vec3 x;      // position
		glm::vec3 v;      // velocity
	};

	struct Derivative {
		glm::vec3 dx;      // dx/dt = velocity
		glm::vec3 dv;      // dv/dt = acceleration
	};

	glm::vec3 acceleration(const State& state) {
		const float k = 15.0f;
		const float b = 0.1f;
		return -k * state.x - b * state.v;
	}

	Derivative evaluate(const State& initial, float dt, const Derivative& d) {
		State state;
		state.x = initial.x + d.dx * dt;
		state.v = initial.v + d.dv * dt;

		Derivative output;
		output.dx = state.v;
		output.dv = acceleration(state);
		return output;
	}

	// FOURTH-ORDER RUNGE-KUTTA
	void cIntegrator::RK4(glm::vec3& pos, glm::vec3& vel, glm::vec3& acc, float dt)	{
		// TODO: math!
		// For an excellent article on RK4 implementation:
		// https://gafferongames.com/post/integration_basics/

		phys::State state;
		state.x = vel + acc * dt;
		state.v = pos + state.x * dt;

		phys::Derivative a, b, c, d;

		a = evaluate(state, 0.0f, phys::Derivative());
		b = evaluate(state, dt * 0.5f, a);
		c = evaluate(state, dt * 0.5f, b);
		d = evaluate(state, dt, c);

		glm::vec3 dxdt = 1.0f / 6.0f *
			(a.dx + 2.0f * (b.dx + c.dx) + d.dx);

		glm::vec3 dvdt = 1.0f / 6.0f *
			(a.dv + 2.0f * (b.dv + c.dv) + d.dv);

		pos = state.x + dxdt * dt;
		vel = state.v + dvdt * dt;
	}

	void cIntegrator::RK4(glm::vec3& pos, glm::vec3& vel, glm::vec3& acc, const glm::vec3& appliedAcc, float dt) {
		// TODO: math!
		// For an excellent article on RK4 implementation:
		// https://gafferongames.com/post/integration_basics/

		// convert accumulated forces to an acceleration vector.
		glm::vec3 accel = acc;
		//accel = ball.Forces * INV_BALL_MASS;

		// if above ground apply gravity.
		if (pos.y > 0.1f)
		{
			accel += appliedAcc;
		}

		// this code based on the Essential Mathematics for Games example code.
		//D3DXVECTOR3 xk1 = ball.Velocity * dt;
		//D3DXVECTOR3 vk1 = accel * dt;
		glm::vec3 xk1 = vel;			xk1 *= dt;
		glm::vec3 vk1 = accel;			vk1 *= dt;

		//D3DXVECTOR3 midVelocity = ball.Velocity + 0.5f * vk1;
		//D3DXVECTOR3 xk2 = dt * midVelocity;
		//D3DXVECTOR3 vk2 = dt * accel;
		glm::vec3 midVelocity = vel;	midVelocity += (0.5f * vk1);
		glm::vec3 xk2 = midVelocity;	xk2 *= dt;
		glm::vec3 vk2 = accel;			vk2 *= dt;

		//midVelocity = ball.Velocity + 0.5f * vk2;
		//D3DXVECTOR3 xk3 = dt * midVelocity;
		//D3DXVECTOR3 vk3 = dt * accel;
		midVelocity = vel;				midVelocity += (0.5f * vk2);
		glm::vec3 xk3 = midVelocity;	xk3 *= dt;
		glm::vec3 vk3 = accel;			vk3 *= dt;

		//midVelocity = ball.Velocity + vk3;
		//D3DXVECTOR3 xk4 = dt * midVelocity;
		//D3DXVECTOR3 vk4 = dt * accel;
		midVelocity = vel;				midVelocity += vk3;
		glm::vec3 xk4 = midVelocity;	xk4 *= dt;
		glm::vec3 vk4 = accel;			vk4 *= dt;

		// update position and velocity.
		pos += (xk1 + 2.0f * xk2 + 2.0f * xk3 + xk4) / 6.0f;
		vel += (vk1 + 2.0f * vk2 + 2.0f * vk3 + vk4) / 6.0f;

		return;

		//Prior Attempt, oscillates all nonstatic bodies around 0, 0, 0.
		//phys::State state;
		//state.v = vel; // +(acc + appliedAcc) * dt;
		//state.x = pos; // +state.v * dt;

		//acc += appliedAcc * dt;

		//phys::Derivative a, b, c, d;

		//a = evaluate(state, 0.0f, phys::Derivative());
		//b = evaluate(state, dt * 0.5f, a);
		//c = evaluate(state, dt * 0.5f, b);
		//d = evaluate(state, dt, c);

		//glm::vec3 dxdt = (a.dx + 2.0f * (b.dx + c.dx) + d.dx) / 6.f;
		//glm::vec3 dvdt = (a.dv + 2.0f * (b.dv + c.dv) + d.dv) / 6.f;

		//pos += dxdt * dt;
		//vel += dvdt * dt;
	}
}