/**
 * Component to load a list of buttons corresponding to LED strip
 * animations
 * 
 * - Available animations are fetched from /animations/get.
 * - Each button triggers a GET request to /animations/select?id=[animationId]
 */

import React, { Component } from "react";
import { Button, Typography, Grid } from '@material-ui/core';

class AnimationButtonList extends Component {
  constructor() {
    super();

    this.state = {
      animations: [], 
      selectedAnimationId: null
    };
  }

  // Fetch all available animations after the component loads
  componentDidMount() {
    fetch('/animations/get')
      .then((response) => response.json())
      .then((data) => 
        this.setState({
          animations: data.animations
        })
      );
  }

  // Send GET request to API to start playing an animation
  selectAnimation(id) {
    fetch('/animations/select?id=' + id)
      .then((response) => response.json())
      .then((data) => 
        this.setState({
          selectedAnimationId: data.result.id
        })
      );
  }

  render() {
    return (
      <div>
        <Grid container direction="column">
          <Typography variant="title" color="inherit">
            Animations
          </Typography>
          {this.state.animations.map((animation) =>
            <Grid item>
              <Button variant="contained" color="primary" onClick={() => { this.selectAnimation(animation.id) }}>
                {animation.name}
              </Button>
            </Grid>
          )}
        </Grid>
      </div>
      
    );
  }
}

export default AnimationButtonList;
