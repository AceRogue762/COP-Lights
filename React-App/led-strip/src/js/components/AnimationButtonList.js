/**
 * Component to load a list of buttons corresponding to LED strip
 * animations
 * 
 * - Available animations are fetched from /api/animations/get.
 * - Each button triggers a GET request to /api/animations/select?id=[animationId]
 */

import React, { Component } from "react";
import { Button, ButtonGroup } from '@material-ui/core';
import { withStyles } from "@material-ui/core/styles";
import Typography from "@material-ui/core/Typography";

const WhiteTextTypography = withStyles({
  root: {
    color: "#FFFFFF"
  }
})(Typography);

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
    fetch('/api/animations/get')
      .then((response) => response.json())
      .then((data) => 
        this.setState({
          animations: data.animations
        })
      );
  }

  // Send GET request to API to start playing an animation
  selectAnimation(id) {
    fetch('/api/animations/select?id=' + id)
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
        <WhiteTextTypography variant="h5" color="textSecondary" gutterBottom>
          Animations
        </WhiteTextTypography>
        <ButtonGroup 
          orientation="vertical" 
          color="primary" 
          variant="contained"
          fullWidth
          >
          {this.state.animations.map((animation) =>
            <Button onClick={() => { this.selectAnimation(animation.id) }}>
              {animation.name}
            </Button>
          )}
        </ButtonGroup>
      </div>
      
    );
  }
}

export default AnimationButtonList;
