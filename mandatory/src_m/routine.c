/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omoudni <omoudni@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/16 18:10:55 by omoudni           #+#    #+#             */
/*   Updated: 2022/07/17 06:12:25 by omoudni          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes_m/philo.h"

void	handle_one(t_philo *philo)
{
	if (!philo->data->t_die || get_timestamp(philo->data->t_launch) >= philo->data->t_die)
		get_message(philo, "died\n", 0);
	else
	{
		get_message(philo, "has taken a fork\n", 0);
		usleep((philo->data->t_die - get_timestamp(philo->data->t_launch)) * 1000);
		get_message(philo, "died\n", 0);
	}
}

int	think(t_philo *philo)
{
	if (stop_simulation(philo, philo->data->n_eat, NULL, 0))
		return (1);
	pthread_mutex_lock(&((philo->data->sdrei)[4]));
	if (philo->data->i && (philo->id % 2))
	{
		philo->data->i--;
		pthread_mutex_unlock(&((philo->data->sdrei)[4]));
		if (get_message(philo, "is thinking", 0))
			return (1);
		usleep(philo->data->t_eat * 1000 - 10);
	}
	else
	{
		pthread_mutex_unlock(&((philo->data->sdrei)[4]));
		if (get_message(philo, "is thinking", 0))
			return (1);
	}
	while (philo->data->mut[philo->id - 1].__data.__lock && philo->data->mut[philo->next->id - 1].__data.__lock)
	{
		if (stop_simulation(philo, philo->data->n_eat, NULL, 0))
			return (1);
		usleep(100);
	}
	return (0);
}

int	eat(t_philo *philo)
{
		if (stop_simulation(philo, philo->data->n_eat, NULL, 0))
			return (1);
		pthread_mutex_lock(&philo->data->mut[philo->id - 1]);
		if(get_message(philo, "has taken a fork", 0))
			return(pthread_mutex_unlock(&philo->data->mut[philo->id - 1]), 1);
		pthread_mutex_lock(&philo->data->mut[philo->next->id - 1]);
		if (get_message(philo, "has taken a fork", 0))
		{
			pthread_mutex_unlock(&philo->data->mut[philo->next->id - 1]);
			return(pthread_mutex_unlock(&philo->data->mut[philo->id - 1]), 1);
		}
		if (eat_sup_die(philo))
			return (1);
		philo->n_eaten++;
		if (philo->n_eaten == philo->data->n_eat)
		{
			pthread_mutex_lock(&(philo->data->sdrei[3]));
			philo->data->all_eaten++;
			if (philo->data->all_eaten == philo->data->n_phil)
			{
				pthread_mutex_unlock(&(philo->data->sdrei[3]));
				if (get_message(philo, "is eating", 1))
				{
					pthread_mutex_unlock(&philo->data->mut[philo->next->id - 1]);
					pthread_mutex_unlock(&philo->data->mut[philo->id - 1]);
					return (1);
				}
				else
				{	
					usleep(philo->data->t_eat * 1000);
					pthread_mutex_unlock(&philo->data->mut[philo->next->id - 1]);
					pthread_mutex_unlock(&philo->data->mut[philo->id - 1]);
					return (1);
				}
			}
			pthread_mutex_unlock(&(philo->data->sdrei[3]));
		}
		if (get_message(philo, "is eating", 0))
		{
					pthread_mutex_unlock(&philo->data->mut[philo->next->id - 1]);
					pthread_mutex_unlock(&philo->data->mut[philo->id - 1]);
					return (1);
		}
		gettimeofday(&philo->lm_time, NULL);
		usleep(philo->data->t_eat);
		pthread_mutex_unlock(&philo->data->mut[philo->next->id - 1]);
		pthread_mutex_unlock(&philo->data->mut[philo->id - 1]);
		return (0);
}

int my_sleep(t_philo *philo)
{
	UINT diff;

	if (stop_simulation(philo, philo->data->n_eat, NULL, 0))
		return (1);
	diff = philo->data->t_die - get_timestamp(philo->lm_time);
	if (diff <= philo->data->t_sleep)
	{
		usleep(diff * 1000 - 20);
		get_message(philo, "is dead", 0);
		return (1);
	}
	else
	{
		if (get_message(philo, "is sleeping", 0));
			return (1);
		usleep(philo->data->t_sleep * 1000 - 10);
	}
	return (0);
}

void	*routine(void *args)
{
	t_philo	*philo;

	philo = (t_philo *)args;
	if (philo->data->n_phil == 1)
		return (handle_one(philo), NULL);
	pthread_mutex_lock(&((philo->data->sdrei)[2]));
	pthread_mutex_unlock(&((philo->data->sdrei)[2]));
	while (1)
	{
		pthread_mutex_lock(&((philo->data->sdrei)[4]));
		if (!philo->data->i || (philo->data->i && (philo->id % 2)))
		{
			pthread_mutex_unlock(&((philo->data->sdrei)[4]));
			if (think(philo))
				return (NULL);
		}
		else
			pthread_mutex_unlock(&((philo->data->sdrei)[4]));
		if (eat(philo))
			return (NULL);
		if (my_sleep(philo))
			return (NULL);
	}
	return (NULL);
}